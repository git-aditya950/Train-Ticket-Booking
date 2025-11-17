#ifndef SEATALLOCATIONSERVICE_H
#define SEATALLOCATIONSERVICE_H

#include <string>
#include <vector>
#include "models/Passenger.h"

class SeatAllocationService {
public:
    SeatAllocationService();
    
    // Seat Assignment Logic
    bool assignSeats(std::vector<Passenger>& passengers, 
                    const std::string& classCode,
                    const std::string& trainNumber);
    
private:
    std::string generateCoachPrefix(const std::string& classCode);
    int getRandomCoachNumber();
    std::string getBerthType(int seatNumber, const std::string& classCode);
    int nextSeatNumber;
};

#endif // SEATALLOCATIONSERVICE_H
