#include "models/Train.h"
#include <sstream>

// TrainAvailability Implementation
TrainAvailability::TrainAvailability() 
    : totalSeats(0), availableSeats(0), price(0.0) {}

TrainAvailability::TrainAvailability(const std::string& classCode, int totalSeats, 
                                   int availableSeats, double price)
    : classCode(classCode), totalSeats(totalSeats), 
      availableSeats(availableSeats), price(price) {}

std::string TrainAvailability::getStatus() const {
    if (availableSeats > 0) {
        return "AVL " + std::to_string(availableSeats);
    } else if (availableSeats == 0) {
        return "WL 0";
    } else {
        return "WL " + std::to_string(-availableSeats);
    }
}

nlohmann::json TrainAvailability::toJson() const {
    return {
        {"class", classCode},
        {"status", getStatus()},
        {"price", price},
        {"availableSeats", availableSeats}
    };
}

TrainAvailability TrainAvailability::fromJson(const nlohmann::json& json) {
    TrainAvailability avail;
    if (json.contains("class")) avail.classCode = json["class"].get<std::string>();
    if (json.contains("totalSeats")) avail.totalSeats = json["totalSeats"].get<int>();
    if (json.contains("availableSeats")) avail.availableSeats = json["availableSeats"].get<int>();
    if (json.contains("price")) avail.price = json["price"].get<double>();
    return avail;
}

// Train Implementation
Train::Train() {}

Train::Train(const std::string& number, const std::string& name)
    : trainNumber(number), trainName(name) {}

std::string Train::getTrainNumber() const { return trainNumber; }
std::string Train::getTrainName() const { return trainName; }
std::string Train::getFromStation() const { return fromStation; }
std::string Train::getToStation() const { return toStation; }
std::string Train::getDepartureTime() const { return departureTime; }
std::string Train::getArrivalTime() const { return arrivalTime; }
std::string Train::getDuration() const { return duration; }
std::vector<TrainAvailability> Train::getAvailability() const { return availability; }

void Train::setFromStation(const std::string& from) { fromStation = from; }
void Train::setToStation(const std::string& to) { toStation = to; }
void Train::setDepartureTime(const std::string& time) { departureTime = time; }
void Train::setArrivalTime(const std::string& time) { arrivalTime = time; }
void Train::setDuration(const std::string& dur) { duration = dur; }
void Train::setAvailability(const std::vector<TrainAvailability>& avail) { 
    availability = avail; 
}

void Train::addAvailability(const TrainAvailability& avail) {
    availability.push_back(avail);
}

bool Train::updateAvailability(const std::string& classCode, int seatsToBook) {
    for (auto& avail : availability) {
        if (avail.classCode == classCode) {
            if (avail.availableSeats >= seatsToBook) {
                avail.availableSeats -= seatsToBook;
                return true;
            }
            return false;
        }
    }
    return false;
}

int Train::getAvailableSeats(const std::string& classCode) const {
    for (const auto& avail : availability) {
        if (avail.classCode == classCode) {
            return avail.availableSeats;
        }
    }
    return 0;
}

double Train::getPrice(const std::string& classCode) const {
    for (const auto& avail : availability) {
        if (avail.classCode == classCode) {
            return avail.price;
        }
    }
    return 0.0;
}

nlohmann::json Train::toJson() const {
    nlohmann::json availJson = nlohmann::json::array();
    for (const auto& avail : availability) {
        availJson.push_back(avail.toJson());
    }
    
    return {
        {"trainNumber", trainNumber},
        {"trainName", trainName},
        {"from", fromStation},
        {"to", toStation},
        {"departureTime", departureTime},
        {"arrivalTime", arrivalTime},
        {"duration", duration},
        {"availability", availJson}
    };
}

Train Train::fromJson(const nlohmann::json& json) {
    Train train;
    
    if (json.contains("trainNumber")) train.trainNumber = json["trainNumber"].get<std::string>();
    if (json.contains("trainName")) train.trainName = json["trainName"].get<std::string>();
    if (json.contains("from")) train.fromStation = json["from"].get<std::string>();
    if (json.contains("to")) train.toStation = json["to"].get<std::string>();
    if (json.contains("departureTime")) train.departureTime = json["departureTime"].get<std::string>();
    if (json.contains("arrivalTime")) train.arrivalTime = json["arrivalTime"].get<std::string>();
    if (json.contains("duration")) train.duration = json["duration"].get<std::string>();
    
    if (json.contains("availability")) {
        for (const auto& availJson : json["availability"]) {
            train.availability.push_back(TrainAvailability::fromJson(availJson));
        }
    }
    
    return train;
}
