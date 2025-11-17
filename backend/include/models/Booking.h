#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <vector>
#include "models/Train.h"
#include "models/Passenger.h"
#include <nlohmann/json.hpp>

class Booking {
private:
    std::string bookingId;
    std::string pnr;
    std::string userId;
    Train train;
    std::string classCode;
    double pricePerPassenger;
    double totalFare;
    std::string journeyDate;
    std::string bookingDate;
    std::string status;
    std::vector<Passenger> passengers;

public:
    Booking();
    Booking(const std::string& userId, const Train& train, 
            const std::string& classCode);
    
    // Getters
    std::string getBookingId() const;
    std::string getPnr() const;
    std::string getUserId() const;
    Train getTrain() const;
    std::string getClassCode() const;
    std::string getStatus() const;
    std::vector<Passenger> getPassengers() const;
    double getTotalFare() const;
    double getPricePerPassenger() const;
    std::string getJourneyDate() const;
    std::string getBookingDate() const;
    
    // Setters
    void setBookingId(const std::string& id);
    void setPnr(const std::string& pnr);
    void addPassenger(const Passenger& passenger);
    void setPassengers(const std::vector<Passenger>& passengers);
    void setJourneyDate(const std::string& date);
    void setStatus(const std::string& status);
    void setPricePerPassenger(double price);
    
    // Business Logic
    void calculateTotalFare();
    std::string generatePnr();
    
    // Validation
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Serialization
    nlohmann::json toJson(bool includePassengers = true) const;
    static Booking fromJson(const nlohmann::json& json);
};

#endif // BOOKING_H
