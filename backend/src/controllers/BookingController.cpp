#include "controllers/BookingController.h"
#include "models/User.h"
#include "utils/DataStore.h"
#include <iostream>

BookingController::BookingController() {}

bool BookingController::validateBookingInput(const nlohmann::json& data, std::string& error) {
    if (!data.contains("train")) {
        error = "Train information is required";
        return false;
    }
    if (!data.contains("selectedClass")) {
        error = "Class selection is required";
        return false;
    }
    if (!data.contains("journeyDate")) {
        error = "Journey date is required";
        return false;
    }
    if (!data.contains("passengers") || !data["passengers"].is_array()) {
        error = "Passengers list is required";
        return false;
    }
    if (data["passengers"].empty()) {
        error = "At least one passenger is required";
        return false;
    }
    if (data["passengers"].size() > 6) {
        error = "Maximum 6 passengers allowed";
        return false;
    }
    return true;
}

Response BookingController::handleCreateBooking(const Request& request) {
    Response response;
    
    try {
        std::cout << "\n=== Creating New Booking ===" << std::endl;
        std::cout << "Request body: " << request.body.dump() << std::endl;
        
        User* user = static_cast<User*>(request.user);
        if (!user) {
            std::cerr << "Error: User not authenticated" << std::endl;
            response.setError("User not authenticated", 401);
            return response;
        }
        
        std::cout << "User authenticated: " << user->getUserId() << std::endl;
        
        // Validate input
        std::string error;
        if (!validateBookingInput(request.body, error)) {
            std::cerr << "Validation error: " << error << std::endl;
            response.setError(error, 400);
            return response;
        }
        
        std::cout << "Input validation passed" << std::endl;
        
        // Parse train
        Train train = Train::fromJson(request.body["train"]);
        std::cout << "Train: " << train.getTrainNumber() << " - " << train.getTrainName() << std::endl;
        
        // Parse class and journey date
        std::string classCode = request.body["selectedClass"]["class"].get<std::string>();
        std::string journeyDate = request.body["journeyDate"].get<std::string>();
        std::cout << "Class: " << classCode << ", Date: " << journeyDate << std::endl;
        
        // Parse passengers
        std::vector<Passenger> passengers;
        for (const auto& passengerJson : request.body["passengers"]) {
            passengers.push_back(Passenger::fromJson(passengerJson));
        }
        std::cout << "Passengers: " << passengers.size() << std::endl;
        
        // Get the stored train to ensure we have updated availability
        DataStore* store = DataStore::getInstance();
        Train* storedTrain = store->findTrainByNumber(train.getTrainNumber());
        
        if (!storedTrain) {
            std::cerr << "Error: Train not found - " << train.getTrainNumber() << std::endl;
            response.setError("Train not found", 404);
            return response;
        }
        
        std::cout << "Train found in datastore" << std::endl;
        
        // Create booking
        Booking* booking = bookingService.createBooking(
            user, *storedTrain, classCode, journeyDate, passengers
        );
        
        if (!booking) {
            std::cerr << "Error: Booking service returned nullptr" << std::endl;
            response.setError("Insufficient seats available or booking failed", 400);
            return response;
        }
        
        std::cout << "Booking created successfully: " << booking->getBookingId() << std::endl;
        
        // Return success
        response.statusCode = 201;
        response.body = {
            {"status", "success"},
            {"message", "Booking confirmed successfully"},
            {"data", booking->toJson(true)}  // Include passengers in response
        };
        
        std::cout << "=== Booking Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in handleCreateBooking: " << e.what() << std::endl;
        response.setError("Internal server error: " + std::string(e.what()), 500);
    }
    
    return response;
}

Response BookingController::handleGetBookings(const Request& request) {
    Response response;
    
    User* user = static_cast<User*>(request.user);
    if (!user) {
        response.setError("User not authenticated", 401);
        return response;
    }
    
    // Get status filter if provided
    std::string status = request.getQueryParam("status");
    
    // Get bookings
    std::vector<Booking> bookings = bookingService.getUserBookings(
        user->getUserId(), status
    );
    
    // Build response
    nlohmann::json bookingsJson = nlohmann::json::array();
    for (const auto& booking : bookings) {
        bookingsJson.push_back(booking.toJson(true));  // Include passengers
    }
    
    response.body = {
        {"status", "success"},
        {"count", bookings.size()},
        {"data", bookingsJson}
    };
    
    return response;
}

Response BookingController::handleGetBookingById(const Request& request) {
    Response response;
    
    User* user = static_cast<User*>(request.user);
    if (!user) {
        response.setError("User not authenticated", 401);
        return response;
    }
    
    std::string bookingId = request.getPathParam("bookingId");
    
    DataStore* store = DataStore::getInstance();
    Booking* booking = store->findBookingById(bookingId);
    
    if (!booking) {
        response.setError("Booking not found", 404);
        return response;
    }
    
    // Verify ownership
    if (booking->getUserId() != user->getUserId()) {
        response.setError("Unauthorized access to booking", 403);
        return response;
    }
    
    response.setSuccess(booking->toJson(true));  // Include passengers
    return response;
}

Response BookingController::handleCancelBooking(const Request& request) {
    Response response;
    
    User* user = static_cast<User*>(request.user);
    if (!user) {
        response.setError("User not authenticated", 401);
        return response;
    }
    
    std::string bookingId = request.getPathParam("bookingId");
    
    // Find booking first to get details for refund calculation
    DataStore* store = DataStore::getInstance();
    Booking* booking = store->findBookingById(bookingId);
    
    if (!booking) {
        response.setError("Booking not found", 404);
        return response;
    }
    
    double refundAmount = bookingService.calculateRefund(*booking);
    
    // Cancel booking
    if (!bookingService.cancelBooking(bookingId, user->getUserId())) {
        response.setError("Failed to cancel booking. Already cancelled or not found.", 400);
        return response;
    }
    
    response.body = {
        {"status", "success"},
        {"message", "Booking cancelled successfully"},
        {"data", {
            {"bookingId", bookingId},
            {"pnr", booking->getPnr()},
            {"status", "Cancelled"},
            {"refundAmount", refundAmount}
        }}
    };
    
    return response;
}
