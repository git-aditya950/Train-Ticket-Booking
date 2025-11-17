#ifndef TRAIN_H
#define TRAIN_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class TrainAvailability {
public:
    std::string classCode;
    int totalSeats;
    int availableSeats;
    double price;
    
    TrainAvailability();
    TrainAvailability(const std::string& classCode, int totalSeats, 
                     int availableSeats, double price);
    
    std::string getStatus() const;
    nlohmann::json toJson() const;
    static TrainAvailability fromJson(const nlohmann::json& json);
};

class Train {
private:
    std::string trainNumber;
    std::string trainName;
    std::string fromStation;
    std::string toStation;
    std::string departureTime;
    std::string arrivalTime;
    std::string duration;
    std::vector<TrainAvailability> availability;

public:
    Train();
    Train(const std::string& number, const std::string& name);
    
    // Getters
    std::string getTrainNumber() const;
    std::string getTrainName() const;
    std::string getFromStation() const;
    std::string getToStation() const;
    std::string getDepartureTime() const;
    std::string getArrivalTime() const;
    std::string getDuration() const;
    std::vector<TrainAvailability> getAvailability() const;
    
    // Setters
    void setFromStation(const std::string& from);
    void setToStation(const std::string& to);
    void setDepartureTime(const std::string& time);
    void setArrivalTime(const std::string& time);
    void setDuration(const std::string& duration);
    void setAvailability(const std::vector<TrainAvailability>& avail);
    void addAvailability(const TrainAvailability& avail);
    
    // Availability Management
    bool updateAvailability(const std::string& classCode, int seatsToBook);
    int getAvailableSeats(const std::string& classCode) const;
    double getPrice(const std::string& classCode) const;
    
    // Serialization
    nlohmann::json toJson() const;
    static Train fromJson(const nlohmann::json& json);
};

#endif // TRAIN_H
