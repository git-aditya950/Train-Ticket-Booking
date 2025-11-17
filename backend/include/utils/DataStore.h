#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "models/User.h"
#include "models/Train.h"
#include "models/Booking.h"

class DataStore {
private:
    // Storage containers
    std::unordered_map<std::string, User> usersByEmail;
    std::unordered_map<std::string, User> usersById;
    std::unordered_map<std::string, Train> trains;
    std::unordered_map<std::string, std::vector<std::string>> trainsByRoute;
    std::unordered_map<std::string, Booking> bookingsById;
    std::unordered_map<std::string, std::vector<std::string>> bookingsByUser;
    std::unordered_map<std::string, std::string> pnrToBookingId;
    std::unordered_map<std::string, std::string> activeSessions;
    
    // Thread safety
    std::mutex userMutex;
    std::mutex trainMutex;
    std::mutex bookingMutex;
    std::mutex sessionMutex;
    
    // Singleton
    static DataStore* instance;
    DataStore();

public:
    static DataStore* getInstance();
    
    // User Operations
    bool addUser(const User& user);
    User* findUserByEmail(const std::string& email);
    User* findUserById(const std::string& userId);
    bool updateUser(const User& user);
    bool deleteUser(const std::string& userId);
    
    // Train Operations
    bool addTrain(const Train& train);
    Train* findTrainByNumber(const std::string& trainNumber);
    std::vector<Train> searchTrains(const std::string& from, const std::string& to);
    bool updateTrain(const Train& train);
    
    // Booking Operations
    bool addBooking(const Booking& booking);
    Booking* findBookingById(const std::string& bookingId);
    Booking* findBookingByPnr(const std::string& pnr);
    std::vector<Booking> findBookingsByUser(const std::string& userId);
    bool updateBooking(const Booking& booking);
    bool deleteBooking(const std::string& bookingId);
    
    // Session Operations
    void addSession(const std::string& token, const std::string& userId);
    std::string getUserIdFromToken(const std::string& token);
    void removeSession(const std::string& token);
    
    // Initialize with sample data
    void initializeSampleData();
};

#endif // DATASTORE_H
