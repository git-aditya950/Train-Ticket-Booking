#ifndef BOOKINGSERVICE_H
#define BOOKINGSERVICE_H

#include <string>
#include <vector>
#include "models/User.h"
#include "models/Train.h"
#include "models/Booking.h"
#include "models/Passenger.h"

class BookingService {
public:
    BookingService();
    
    // Business Logic
    Booking* createBooking(User* user, 
                          const Train& train,
                          const std::string& classCode,
                          const std::string& journeyDate,
                          const std::vector<Passenger>& passengers);
    
    std::vector<Booking> getUserBookings(const std::string& userId,
                                        const std::string& status = "");
    
    bool cancelBooking(const std::string& bookingId, 
                      const std::string& userId);
    
    double calculateRefund(const Booking& booking);
    
private:
    bool validateBookingRules(const Booking& booking, std::string& error);
    bool updateTrainAvailability(const std::string& trainNumber,
                                const std::string& classCode,
                                int seatsToDeduct);
    std::string generateBookingId();
};

#endif // BOOKINGSERVICE_H
