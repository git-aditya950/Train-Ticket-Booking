#include "utils/DataStore.h"
#include <algorithm>
#include <iostream>

DataStore* DataStore::instance = nullptr;

DataStore::DataStore() {}

DataStore* DataStore::getInstance() {
    if (instance == nullptr) {
        instance = new DataStore();
    }
    return instance;
}

// User Operations
bool DataStore::addUser(const User& user) {
    std::lock_guard<std::mutex> lock(userMutex);
    
    if (usersByEmail.find(user.getEmail()) != usersByEmail.end()) {
        return false; // Email already exists
    }
    
    usersByEmail[user.getEmail()] = user;
    usersById[user.getUserId()] = user;
    return true;
}

User* DataStore::findUserByEmail(const std::string& email) {
    std::lock_guard<std::mutex> lock(userMutex);
    
    auto it = usersByEmail.find(email);
    if (it != usersByEmail.end()) {
        return &(it->second);
    }
    return nullptr;
}

User* DataStore::findUserById(const std::string& userId) {
    std::lock_guard<std::mutex> lock(userMutex);
    
    auto it = usersById.find(userId);
    if (it != usersById.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool DataStore::updateUser(const User& user) {
    std::lock_guard<std::mutex> lock(userMutex);
    
    auto it = usersById.find(user.getUserId());
    if (it != usersById.end()) {
        it->second = user;
        usersByEmail[user.getEmail()] = user;
        return true;
    }
    return false;
}

bool DataStore::deleteUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(userMutex);
    
    auto it = usersById.find(userId);
    if (it != usersById.end()) {
        std::string email = it->second.getEmail();
        usersById.erase(it);
        usersByEmail.erase(email);
        return true;
    }
    return false;
}

// Train Operations
bool DataStore::addTrain(const Train& train) {
    std::lock_guard<std::mutex> lock(trainMutex);
    
    trains[train.getTrainNumber()] = train;
    
    // Add to route index
    std::string routeKey = train.getFromStation() + "|" + train.getToStation();
    trainsByRoute[routeKey].push_back(train.getTrainNumber());
    
    return true;
}

Train* DataStore::findTrainByNumber(const std::string& trainNumber) {
    std::lock_guard<std::mutex> lock(trainMutex);
    
    auto it = trains.find(trainNumber);
    if (it != trains.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::vector<Train> DataStore::searchTrains(const std::string& from, const std::string& to) {
    std::lock_guard<std::mutex> lock(trainMutex);
    
    std::vector<Train> results;
    
    // Search using route index
    std::string routeKey = from + "|" + to;
    auto it = trainsByRoute.find(routeKey);
    
    if (it != trainsByRoute.end()) {
        for (const auto& trainNumber : it->second) {
            auto trainIt = trains.find(trainNumber);
            if (trainIt != trains.end()) {
                results.push_back(trainIt->second);
            }
        }
    }
    
    return results;
}

bool DataStore::updateTrain(const Train& train) {
    std::lock_guard<std::mutex> lock(trainMutex);
    
    auto it = trains.find(train.getTrainNumber());
    if (it != trains.end()) {
        it->second = train;
        return true;
    }
    return false;
}

// Booking Operations
bool DataStore::addBooking(const Booking& booking) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    bookingsById[booking.getBookingId()] = booking;
    bookingsByUser[booking.getUserId()].push_back(booking.getBookingId());
    pnrToBookingId[booking.getPnr()] = booking.getBookingId();
    
    return true;
}

Booking* DataStore::findBookingById(const std::string& bookingId) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    auto it = bookingsById.find(bookingId);
    if (it != bookingsById.end()) {
        return &(it->second);
    }
    return nullptr;
}

Booking* DataStore::findBookingByPnr(const std::string& pnr) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    auto it = pnrToBookingId.find(pnr);
    if (it != pnrToBookingId.end()) {
        std::string bookingId = it->second;
        auto bookingIt = bookingsById.find(bookingId);
        if (bookingIt != bookingsById.end()) {
            return &(bookingIt->second);
        }
    }
    return nullptr;
}

std::vector<Booking> DataStore::findBookingsByUser(const std::string& userId) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    std::vector<Booking> results;
    auto it = bookingsByUser.find(userId);
    
    if (it != bookingsByUser.end()) {
        for (const auto& bookingId : it->second) {
            auto bookingIt = bookingsById.find(bookingId);
            if (bookingIt != bookingsById.end()) {
                results.push_back(bookingIt->second);
            }
        }
    }
    
    return results;
}

bool DataStore::updateBooking(const Booking& booking) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    auto it = bookingsById.find(booking.getBookingId());
    if (it != bookingsById.end()) {
        it->second = booking;
        return true;
    }
    return false;
}

bool DataStore::deleteBooking(const std::string& bookingId) {
    std::lock_guard<std::mutex> lock(bookingMutex);
    
    auto it = bookingsById.find(bookingId);
    if (it != bookingsById.end()) {
        std::string userId = it->second.getUserId();
        std::string pnr = it->second.getPnr();
        
        bookingsById.erase(it);
        pnrToBookingId.erase(pnr);
        
        auto& userBookings = bookingsByUser[userId];
        userBookings.erase(std::remove(userBookings.begin(), userBookings.end(), bookingId), 
                          userBookings.end());
        
        return true;
    }
    return false;
}

// Session Operations
void DataStore::addSession(const std::string& token, const std::string& userId) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    activeSessions[token] = userId;
}

std::string DataStore::getUserIdFromToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    
    auto it = activeSessions.find(token);
    if (it != activeSessions.end()) {
        return it->second;
    }
    return "";
}

void DataStore::removeSession(const std::string& token) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    activeSessions.erase(token);
}

// Initialize with sample data
void DataStore::initializeSampleData() {
    std::cout << "Initializing sample train data..." << std::endl;
    
    // Sample trains
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string>> trainData = {
        {"12951", "Mumbai Rajdhani", "New Delhi (NDLS)", "Mumbai (CST)", "17:00", "08:35", "15h 35m"},
        {"12001", "Shatabdi Express", "New Delhi (NDLS)", "Bhopal (BPL)", "06:00", "14:05", "8h 5m"},
        {"12301", "Howrah Rajdhani", "New Delhi (NDLS)", "Howrah (HWH)", "16:55", "09:55", "17h 0m"},
        {"12951", "Mumbai Rajdhani", "Mumbai (CST)", "New Delhi (NDLS)", "17:00", "08:35", "15h 35m"},
        {"12423", "Dibrugarh Rajdhani", "New Delhi (NDLS)", "Dibrugarh (DBRG)", "11:00", "14:55", "51h 55m"},
        {"12429", "Lucknow Shatabdi", "New Delhi (NDLS)", "Lucknow (LKO)", "06:10", "12:35", "6h 25m"},
        {"12301", "Howrah Rajdhani", "Howrah (HWH)", "New Delhi (NDLS)", "16:55", "09:55", "17h 0m"},
        {"12009", "Shatabdi Express", "Mumbai (CST)", "Ahmedabad (ADI)", "06:00", "13:00", "7h 0m"}
    };
    
    for (const auto& td : trainData) {
        Train train(std::get<0>(td), std::get<1>(td));
        train.setFromStation(std::get<2>(td));
        train.setToStation(std::get<3>(td));
        train.setDepartureTime(std::get<4>(td));
        train.setArrivalTime(std::get<5>(td));
        train.setDuration(std::get<6>(td));
        
        // Add availability for different classes
        train.addAvailability(TrainAvailability("SL", 200, 150, 1200));
        train.addAvailability(TrainAvailability("3A", 64, 45, 2500));
        train.addAvailability(TrainAvailability("2A", 48, 22, 3500));
        train.addAvailability(TrainAvailability("1A", 24, 8, 4800));
        train.addAvailability(TrainAvailability("CC", 80, 60, 1800));
        train.addAvailability(TrainAvailability("EC", 40, 20, 3200));
        
        addTrain(train);
    }
    
    std::cout << "Initialized " << trains.size() << " trains" << std::endl;
}
