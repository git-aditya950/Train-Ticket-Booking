#include "services/BookingService.h"
#include "services/SeatAllocationService.h"
#include "utils/DataStore.h"
#include <random>
#include <sstream>
#include <iostream>

BookingService::BookingService() {}

std::string BookingService::generateBookingId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    std::ostringstream oss;
    oss << "booking_" << dis(gen);
    return oss.str();
}

bool BookingService::validateBookingRules(const Booking& booking, std::string& error) {
    // Check passenger count
    if (booking.getPassengers().empty()) {
        error = "At least one passenger is required";
        return false;
    }
    
    if (booking.getPassengers().size() > 6) {
        error = "Maximum 6 passengers allowed per booking";
        return false;
    }
    
    // Validate each passenger
    for (const auto& passenger : booking.getPassengers()) {
        if (!passenger.isValid()) {
            error = "Invalid passenger data";
            return false;
        }
    }
    
    return true;
}

bool BookingService::updateTrainAvailability(const std::string& trainNumber,
                                            const std::string& classCode,
                                            int seatsToDeduct) {
    DataStore* store = DataStore::getInstance();
    Train* train = store->findTrainByNumber(trainNumber);
    
    if (!train) {
        return false;
    }
    
    if (!train->updateAvailability(classCode, seatsToDeduct)) {
        return false;
    }
    
    return store->updateTrain(*train);
}

Booking* BookingService::createBooking(User* user, 
                                      const Train& train,
                                      const std::string& classCode,
                                      const std::string& journeyDate,
                                      const std::vector<Passenger>& passengers) {
    if (!user) {
        return nullptr;
    }
    
    DataStore* store = DataStore::getInstance();
    
    // Check seat availability
    int availableSeats = train.getAvailableSeats(classCode);
    if (availableSeats < (int)passengers.size()) {
        std::cerr << "Insufficient seats. Available: " << availableSeats 
                  << ", Requested: " << passengers.size() << std::endl;
        return nullptr;
    }
    
    // Create booking
    Booking booking(user->getUserId(), train, classCode);
    booking.setBookingId(generateBookingId());
    booking.setPnr(booking.generatePnr());
    booking.setJourneyDate(journeyDate);
    
    // Set price and add passengers
    double price = train.getPrice(classCode);
    booking.setPricePerPassenger(price);
    
    std::vector<Passenger> passengersCopy = passengers;
    
    // Assign seats
    SeatAllocationService seatService;
    if (!seatService.assignSeats(passengersCopy, classCode, train.getTrainNumber())) {
        std::cerr << "Failed to assign seats" << std::endl;
        return nullptr;
    }
    
    for (const auto& passenger : passengersCopy) {
        booking.addPassenger(passenger);
    }
    
    booking.calculateTotalFare();
    
    // Validate booking
    std::string error;
    if (!validateBookingRules(booking, error)) {
        std::cerr << "Booking validation failed: " << error << std::endl;
        return nullptr;
    }
    
    // Update train availability
    if (!updateTrainAvailability(train.getTrainNumber(), classCode, passengers.size())) {
        std::cerr << "Failed to update train availability" << std::endl;
        return nullptr;
    }
    
    // Save booking
    if (!store->addBooking(booking)) {
        std::cerr << "Failed to save booking" << std::endl;
        return nullptr;
    }
    
    return store->findBookingById(booking.getBookingId());
}

std::vector<Booking> BookingService::getUserBookings(const std::string& userId,
                                                     const std::string& status) {
    DataStore* store = DataStore::getInstance();
    std::vector<Booking> bookings = store->findBookingsByUser(userId);
    
    // Filter by status if provided
    if (!status.empty()) {
        std::vector<Booking> filtered;
        for (const auto& booking : bookings) {
            if (booking.getStatus() == status) {
                filtered.push_back(booking);
            }
        }
        return filtered;
    }
    
    return bookings;
}

bool BookingService::cancelBooking(const std::string& bookingId, 
                                   const std::string& userId) {
    DataStore* store = DataStore::getInstance();
    Booking* booking = store->findBookingById(bookingId);
    
    if (!booking) {
        return false;
    }
    
    // Verify ownership
    if (booking->getUserId() != userId) {
        return false;
    }
    
    // Check if already cancelled
    if (booking->getStatus() == "Cancelled") {
        return false;
    }
    
    // Update status
    booking->setStatus("Cancelled");
    
    // Restore train availability
    Train train = booking->getTrain();
    int passengerCount = booking->getPassengers().size();
    
    Train* storedTrain = store->findTrainByNumber(train.getTrainNumber());
    if (storedTrain) {
        // Restore seats
        for (auto& avail : storedTrain->getAvailability()) {
            if (avail.classCode == booking->getClassCode()) {
                avail.availableSeats += passengerCount;
                break;
            }
        }
        store->updateTrain(*storedTrain);
    }
    
    return store->updateBooking(*booking);
}

double BookingService::calculateRefund(const Booking& booking) {
    if (booking.getStatus() != "Cancelled") {
        return 0.0;
    }
    
    // 80% refund policy
    return booking.getTotalFare() * 0.8;
}
