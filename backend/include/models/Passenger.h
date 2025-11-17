#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>
#include <nlohmann/json.hpp>

class Passenger {
private:
    std::string name;
    int age;
    std::string gender;
    std::string berthPreference;
    std::string assignedSeat;
    std::string assignedBerth;

public:
    Passenger();
    Passenger(const std::string& name, int age, 
              const std::string& gender, const std::string& berth);
    
    // Getters
    std::string getName() const;
    int getAge() const;
    std::string getGender() const;
    std::string getBerthPreference() const;
    std::string getAssignedSeat() const;
    std::string getAssignedBerth() const;
    
    // Setters
    void setAssignedSeat(const std::string& seat);
    void setAssignedBerth(const std::string& berth);
    
    // Validation
    bool isValid() const;
    
    // Serialization
    nlohmann::json toJson() const;
    static Passenger fromJson(const nlohmann::json& json);
};

#endif // PASSENGER_H
