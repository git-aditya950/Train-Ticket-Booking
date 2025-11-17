#include "models/Booking.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>

Booking::Booking() : pricePerPassenger(0.0), totalFare(0.0), status("Confirmed") {}

Booking::Booking(const std::string& userId, const Train& train, 
                 const std::string& classCode)
    : userId(userId), train(train), classCode(classCode), 
      pricePerPassenger(0.0), totalFare(0.0), status("Confirmed") {
    
    // Generate timestamp
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%S.000Z");
    bookingDate = oss.str();
}

std::string Booking::getBookingId() const { return bookingId; }
std::string Booking::getPnr() const { return pnr; }
std::string Booking::getUserId() const { return userId; }
Train Booking::getTrain() const { return train; }
std::string Booking::getClassCode() const { return classCode; }
std::string Booking::getStatus() const { return status; }
std::vector<Passenger> Booking::getPassengers() const { return passengers; }
double Booking::getTotalFare() const { return totalFare; }
double Booking::getPricePerPassenger() const { return pricePerPassenger; }
std::string Booking::getJourneyDate() const { return journeyDate; }
std::string Booking::getBookingDate() const { return bookingDate; }

void Booking::setBookingId(const std::string& id) { bookingId = id; }
void Booking::setPnr(const std::string& p) { pnr = p; }
void Booking::addPassenger(const Passenger& passenger) { 
    passengers.push_back(passenger); 
}
void Booking::setPassengers(const std::vector<Passenger>& p) { passengers = p; }
void Booking::setJourneyDate(const std::string& date) { journeyDate = date; }
void Booking::setStatus(const std::string& s) { status = s; }
void Booking::setPricePerPassenger(double price) { pricePerPassenger = price; }

void Booking::calculateTotalFare() {
    totalFare = pricePerPassenger * passengers.size();
}

std::string Booking::generatePnr() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis1(100, 999);
    std::uniform_int_distribution<> dis2(1000000, 9999999);
    
    std::ostringstream oss;
    oss << dis1(gen) << "-" << dis2(gen);
    return oss.str();
}

bool Booking::isValid() const {
    if (userId.empty() || classCode.empty() || journeyDate.empty()) {
        return false;
    }
    if (passengers.empty() || passengers.size() > 6) {
        return false;
    }
    for (const auto& passenger : passengers) {
        if (!passenger.isValid()) {
            return false;
        }
    }
    return true;
}

std::string Booking::getValidationErrors() const {
    if (userId.empty()) return "User ID is required";
    if (classCode.empty()) return "Class is required";
    if (journeyDate.empty()) return "Journey date is required";
    if (passengers.empty()) return "At least one passenger is required";
    if (passengers.size() > 6) return "Maximum 6 passengers allowed";
    
    for (const auto& passenger : passengers) {
        if (!passenger.isValid()) {
            return "Invalid passenger data";
        }
    }
    
    return "";
}

nlohmann::json Booking::toJson(bool includePassengers) const {
    nlohmann::json j = {
        {"bookingId", bookingId},
        {"pnr", pnr},
        {"userId", userId},
        {"train", train.toJson()},
        {"selectedClass", {
            {"class", classCode},
            {"price", pricePerPassenger}
        }},
        {"journeyDate", journeyDate},
        {"totalFare", totalFare},
        {"bookingDate", bookingDate},
        {"status", status}
    };
    
    if (includePassengers) {
        nlohmann::json passengersJson = nlohmann::json::array();
        for (const auto& passenger : passengers) {
            passengersJson.push_back(passenger.toJson());
        }
        j["passengers"] = passengersJson;
    }
    
    return j;
}

Booking Booking::fromJson(const nlohmann::json& json) {
    Booking booking;
    
    if (json.contains("bookingId")) booking.bookingId = json["bookingId"].get<std::string>();
    if (json.contains("pnr")) booking.pnr = json["pnr"].get<std::string>();
    if (json.contains("userId")) booking.userId = json["userId"].get<std::string>();
    if (json.contains("train")) booking.train = Train::fromJson(json["train"]);
    
    if (json.contains("selectedClass")) {
        const auto& sc = json["selectedClass"];
        if (sc.contains("class")) booking.classCode = sc["class"].get<std::string>();
        if (sc.contains("price")) booking.pricePerPassenger = sc["price"].get<double>();
    }
    
    if (json.contains("journeyDate")) booking.journeyDate = json["journeyDate"].get<std::string>();
    if (json.contains("totalFare")) booking.totalFare = json["totalFare"].get<double>();
    if (json.contains("bookingDate")) booking.bookingDate = json["bookingDate"].get<std::string>();
    if (json.contains("status")) booking.status = json["status"].get<std::string>();
    
    if (json.contains("passengers")) {
        for (const auto& passengerJson : json["passengers"]) {
            booking.passengers.push_back(Passenger::fromJson(passengerJson));
        }
    }
    
    return booking;
}
