#include "services/SeatAllocationService.h"
#include <random>
#include <sstream>

SeatAllocationService::SeatAllocationService() : nextSeatNumber(1) {}

std::string SeatAllocationService::generateCoachPrefix(const std::string& classCode) {
    if (classCode == "SL") return "S";
    if (classCode == "3A") return "B";
    if (classCode == "2A") return "A";
    if (classCode == "1A") return "H";
    if (classCode == "CC") return "H";
    if (classCode == "EC") return "H";
    return "S";
}

int SeatAllocationService::getRandomCoachNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5);
    return dis(gen);
}

std::string SeatAllocationService::getBerthType(int seatNumber, const std::string& classCode) {
    // Berth assignment based on seat number
    int mod = seatNumber % 8;
    
    if (classCode == "SL" || classCode == "3A" || classCode == "2A") {
        switch (mod) {
            case 1: return "Lower";
            case 2: return "Middle";
            case 3: return "Upper";
            case 4: return "Lower";
            case 5: return "Middle";
            case 6: return "Upper";
            case 7: return "Side Lower";
            case 0: return "Side Upper";
            default: return "Lower";
        }
    } else {
        // For CC, EC, 1A (sitting)
        return "Seat";
    }
}

bool SeatAllocationService::assignSeats(std::vector<Passenger>& passengers, 
                                       const std::string& classCode,
                                       const std::string& trainNumber) {
    std::string coachPrefix = generateCoachPrefix(classCode);
    int coachNumber = getRandomCoachNumber();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 72);
    
    int startSeat = dis(gen);
    
    for (size_t i = 0; i < passengers.size(); i++) {
        int seatNumber = startSeat + i;
        
        // Generate seat assignment
        std::ostringstream oss;
        oss << coachPrefix << coachNumber << "-" << seatNumber;
        passengers[i].setAssignedSeat(oss.str());
        
        // Assign berth type
        std::string berthType = getBerthType(seatNumber, classCode);
        passengers[i].setAssignedBerth(berthType);
    }
    
    return true;
}
