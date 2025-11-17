#include "models/Passenger.h"

Passenger::Passenger() : age(0) {}

Passenger::Passenger(const std::string& name, int age, 
                     const std::string& gender, const std::string& berth)
    : name(name), age(age), gender(gender), berthPreference(berth) {}

std::string Passenger::getName() const { return name; }
int Passenger::getAge() const { return age; }
std::string Passenger::getGender() const { return gender; }
std::string Passenger::getBerthPreference() const { return berthPreference; }
std::string Passenger::getAssignedSeat() const { return assignedSeat; }
std::string Passenger::getAssignedBerth() const { return assignedBerth; }

void Passenger::setAssignedSeat(const std::string& seat) { assignedSeat = seat; }
void Passenger::setAssignedBerth(const std::string& berth) { assignedBerth = berth; }

bool Passenger::isValid() const {
    if (name.empty()) return false;
    if (age < 1 || age > 120) return false;
    if (gender.empty()) return false;
    return true;
}

nlohmann::json Passenger::toJson() const {
    nlohmann::json j = {
        {"name", name},
        {"age", age},
        {"gender", gender},
        {"berth", berthPreference}
    };
    
    if (!assignedSeat.empty()) {
        j["assignedSeat"] = assignedSeat;
    }
    if (!assignedBerth.empty()) {
        j["assignedBerth"] = assignedBerth;
    }
    
    return j;
}

Passenger Passenger::fromJson(const nlohmann::json& json) {
    Passenger passenger;
    
    if (json.contains("name")) passenger.name = json["name"].get<std::string>();
    if (json.contains("age")) passenger.age = json["age"].get<int>();
    if (json.contains("gender")) passenger.gender = json["gender"].get<std::string>();
    if (json.contains("berth")) passenger.berthPreference = json["berth"].get<std::string>();
    if (json.contains("assignedSeat")) passenger.assignedSeat = json["assignedSeat"].get<std::string>();
    if (json.contains("assignedBerth")) passenger.assignedBerth = json["assignedBerth"].get<std::string>();
    
    return passenger;
}
