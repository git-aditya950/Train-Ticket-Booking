#ifndef BOOKINGCONTROLLER_H
#define BOOKINGCONTROLLER_H

#include "utils/Request.h"
#include "utils/Response.h"
#include "services/BookingService.h"

class BookingController {
private:
    BookingService bookingService;
    
    bool validateBookingInput(const nlohmann::json& data, std::string& error);

public:
    BookingController();
    
    Response handleCreateBooking(const Request& request);
    Response handleGetBookings(const Request& request);
    Response handleGetBookingById(const Request& request);
    Response handleCancelBooking(const Request& request);
};

#endif // BOOKINGCONTROLLER_H
