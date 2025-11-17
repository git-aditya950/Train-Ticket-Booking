#include "utils/DataStore.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

DataStore* DataStore::instance = nullptr;

DataStore::DataStore() {
    // Initialize random seed for generating varied availability data
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

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
    std::cout << "Initializing comprehensive train data (5000+ trains)..." << std::endl;
    
    // Major stations for route generation
    std::vector<std::string> majorStations = {
        "New Delhi (NDLS)", "Mumbai (CST)", "Kolkata (HWH)", "Chennai (MAS)", "Bangalore (SBC)",
        "Hyderabad (SC)", "Pune (PUNE)", "Ahmedabad (ADI)", "Jaipur (JP)", "Lucknow (LKO)",
        "Bhopal (BPL)", "Indore (INDB)", "Patna (PNBE)", "Ranchi (RNC)", "Bhubaneswar (BBS)",
        "Guwahati (GHY)", "Thiruvananthapuram (TVC)", "Kochi (ERN)", "Goa (MAO)", "Nagpur (NGP)",
        "Visakhapatnam (VSKP)", "Amritsar (ASR)", "Chandigarh (CDG)", "Dehradun (DDN)",
        "Varanasi (BSB)", "Agra (AGC)", "Udaipur (UDZ)", "Jodhpur (JU)", "Mysore (MYS)",
        "Coimbatore (CBE)", "Madurai (MDU)", "Puri (PURI)", "Jammu (JAT)", "Kanpur (CNB)",
        "Allahabad (PRYJ)", "Gorakhpur (GKP)", "Jabalpur (JBP)", "Raipur (R)", "Vijayawada (BZA)",
        "Guntur (GNT)", "Kozhikode (CLT)", "Mangalore (MAQ)", "Hubli (UBL)", "Surat (ST)",
        "Rajkot (RJT)", "Vadodara (BRC)", "Nashik (NK)", "Aurangabad (AWB)", "Solapur (SUR)"
    };
    
    // Train name templates
    std::vector<std::pair<std::string, std::string>> trainTypes = {
        {"Rajdhani", "Rajdhani Express"},
        {"Shatabdi", "Shatabdi Express"},
        {"Duronto", "Duronto Express"},
        {"Express", "Superfast Express"},
        {"Mail", "Mail Express"},
        {"Sampark", "Sampark Kranti"},
        {"Intercity", "Intercity Express"},
        {"Garib", "Garib Rath"},
        {"Humsafar", "Humsafar Express"},
        {"Tejas", "Tejas Express"}
    };
    
    // Time slots for departures
    std::vector<std::string> departureSlots = {
        "00:30", "02:15", "04:00", "05:30", "06:00", "06:45", "07:30", "08:15",
        "09:00", "09:45", "10:30", "11:15", "12:00", "12:45", "13:30", "14:15",
        "15:00", "15:45", "16:30", "17:15", "18:00", "18:45", "19:30", "20:15",
        "21:00", "21:45", "22:30", "23:15"
    };
    
    int trainCounter = 10001;
    int totalTrains = 0;
    
    // Generate trains for all major route combinations
    for (size_t i = 0; i < majorStations.size(); i++) {
        for (size_t j = 0; j < majorStations.size(); j++) {
            if (i == j) continue; // Skip same station routes
            
            std::string from = majorStations[i];
            std::string to = majorStations[j];
            
            // Generate 2-4 trains per route with different timings
            int trainsPerRoute = 2 + (rand() % 3);
            
            for (int k = 0; k < trainsPerRoute; k++) {
                // Select train type
                auto trainType = trainTypes[rand() % trainTypes.size()];
                
                // Generate train number
                std::string trainNumber = std::to_string(trainCounter++);
                
                // Generate train name
                std::string trainName = trainType.second;
                
                // Select departure time
                std::string departureTime = departureSlots[rand() % departureSlots.size()];
                
                // Calculate journey duration (in hours)
                int durationHours = 3 + (rand() % 45); // 3-48 hours
                int durationMinutes = (rand() % 12) * 5; // 0-55 minutes in 5-min increments
                
                // Calculate arrival time
                int depHour = std::stoi(departureTime.substr(0, 2));
                int depMin = std::stoi(departureTime.substr(3, 2));
                
                int totalMinutes = depHour * 60 + depMin + durationHours * 60 + durationMinutes;
                int arrHour = (totalMinutes / 60) % 24;
                int arrMin = totalMinutes % 60;
                
                char arrivalTime[6];
                std::sprintf(arrivalTime, "%02d:%02d", arrHour, arrMin);
                
                // Format duration
                char duration[10];
                std::sprintf(duration, "%dh %dm", durationHours, durationMinutes);
                
                // Create and configure train
                Train train(trainNumber, trainName);
                train.setFromStation(from);
                train.setToStation(to);
                train.setDepartureTime(departureTime);
                train.setArrivalTime(std::string(arrivalTime));
                train.setDuration(std::string(duration));
                
                // Add availability based on train type
                if (trainType.first == "Rajdhani" || trainType.first == "Tejas") {
                    // Premium AC trains
                    train.addAvailability(TrainAvailability("3A", 64, 20 + (rand() % 50), 2200 + (rand() % 800)));
                    train.addAvailability(TrainAvailability("2A", 48, 10 + (rand() % 30), 3200 + (rand() % 1000)));
                    train.addAvailability(TrainAvailability("1A", 24, 2 + (rand() % 15), 4500 + (rand() % 1500)));
                } 
                else if (trainType.first == "Shatabdi") {
                    // Chair car trains
                    train.addAvailability(TrainAvailability("CC", 80, 30 + (rand() % 60), 1200 + (rand() % 800)));
                    train.addAvailability(TrainAvailability("EC", 40, 5 + (rand() % 25), 2500 + (rand() % 800)));
                }
                else if (trainType.first == "Duronto" || trainType.first == "Humsafar") {
                    // Express trains with multiple classes
                    train.addAvailability(TrainAvailability("SL", 200, 50 + (rand() % 150), 800 + (rand() % 500)));
                    train.addAvailability(TrainAvailability("3A", 64, 20 + (rand() % 50), 2000 + (rand() % 800)));
                    train.addAvailability(TrainAvailability("2A", 48, 10 + (rand() % 30), 2800 + (rand() % 900)));
                }
                else if (trainType.first == "Garib") {
                    // Garib Rath - AC budget trains
                    train.addAvailability(TrainAvailability("3A", 120, 40 + (rand() % 80), 1500 + (rand() % 600)));
                }
                else {
                    // Regular Mail/Express trains - All classes
                    int slSeats = 150 + (rand() % 150);
                    int available = slSeats - (rand() % 100);
                    if (available < 0) available = -(rand() % 40); // Waitlist for popular trains
                    
                    train.addAvailability(TrainAvailability("SL", slSeats, available, 600 + (rand() % 700)));
                    train.addAvailability(TrainAvailability("3A", 64, 15 + (rand() % 60), 1700 + (rand() % 900)));
                    train.addAvailability(TrainAvailability("2A", 48, 8 + (rand() % 35), 2400 + (rand() % 1100)));
                    
                    // Some trains have 1A
                    if (rand() % 3 == 0) {
                        train.addAvailability(TrainAvailability("1A", 24, 2 + (rand() % 12), 3500 + (rand() % 1500)));
                    }
                    
                    // Some trains have chair car
                    if (rand() % 4 == 0) {
                        train.addAvailability(TrainAvailability("CC", 80, 20 + (rand() % 60), 1000 + (rand() % 600)));
                    }
                }
                
                addTrain(train);
                totalTrains++;
            }
        }
    }
    
    std::cout << "Initialized " << totalTrains << " trains covering " << majorStations.size() << " stations" << std::endl;
    
    // Now add the specific premium/popular trains with fixed details
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string>> premiumTrainData = {
        // Major Rajdhani & Premium Trains
        {"12001", "Shatabdi Express", "New Delhi (NDLS)", "Bhopal (BPL)", "06:00", "14:05", "8h 5m"},
        {"12002", "Shatabdi Express", "Bhopal (BPL)", "New Delhi (NDLS)", "14:40", "22:50", "8h 10m"},
        {"12951", "Mumbai Rajdhani", "Mumbai (CST)", "New Delhi (NDLS)", "17:00", "08:35", "15h 35m"},
        {"12952", "Mumbai Rajdhani", "New Delhi (NDLS)", "Mumbai (CST)", "17:00", "08:35", "15h 35m"},
        {"12953", "August Kranti Rajdhani", "Mumbai (CST)", "New Delhi (NDLS)", "17:10", "09:43", "16h 33m"},
        {"12954", "August Kranti Rajdhani", "New Delhi (NDLS)", "Mumbai (CST)", "17:10", "09:43", "16h 33m"},
        {"12301", "Howrah Rajdhani", "Kolkata (HWH)", "New Delhi (NDLS)", "16:50", "09:55", "17h 5m"},
        {"12302", "Howrah Rajdhani", "New Delhi (NDLS)", "Kolkata (HWH)", "16:50", "09:55", "17h 5m"},
        {"12313", "Sealdah Rajdhani", "Kolkata (HWH)", "New Delhi (NDLS)", "16:50", "10:05", "17h 15m"},
        {"12314", "Sealdah Rajdhani", "New Delhi (NDLS)", "Kolkata (HWH)", "16:50", "10:05", "17h 15m"},
        {"22691", "Bengaluru Rajdhani", "Bangalore (SBC)", "New Delhi (NDLS)", "20:00", "05:50", "33h 50m"},
        {"22692", "Bengaluru Rajdhani", "New Delhi (NDLS)", "Bangalore (SBC)", "20:00", "05:50", "33h 50m"},
        {"12431", "Trivandrum Rajdhani", "New Delhi (NDLS)", "Thiruvananthapuram (TVC)", "06:16", "10:55", "40h 39m"},
        {"12432", "Trivandrum Rajdhani", "Thiruvananthapuram (TVC)", "New Delhi (NDLS)", "06:16", "10:55", "40h 39m"},
        {"12433", "Kerala Rajdhani", "New Delhi (NDLS)", "Kochi (ERN)", "06:16", "10:55", "40h 39m"},
        {"12434", "Kerala Rajdhani", "Kochi (ERN)", "New Delhi (NDLS)", "06:16", "10:55", "40h 39m"},
        {"12424", "Dibrugarh Rajdhani", "New Delhi (NDLS)", "Guwahati (GHY)", "16:20", "20:50", "28h 30m"},
        {"12423", "Dibrugarh Rajdhani", "Guwahati (GHY)", "New Delhi (NDLS)", "16:20", "20:50", "28h 30m"},
        
        // Duronto Express Trains
        {"12259", "Duronto Express", "Kolkata (HWH)", "Chennai (MAS)", "17:00", "20:10", "27h 10m"},
        {"12260", "Duronto Express", "Chennai (MAS)", "Kolkata (HWH)", "17:00", "20:10", "27h 10m"},
        {"12263", "Pune Duronto", "Pune (PUNE)", "New Delhi (NDLS)", "11:00", "06:55", "19h 55m"},
        {"12264", "Pune Duronto", "New Delhi (NDLS)", "Pune (PUNE)", "11:00", "06:55", "19h 55m"},
        {"12273", "Howrah Duronto", "New Delhi (NDLS)", "Kolkata (HWH)", "12:40", "10:35", "21h 55m"},
        {"12274", "Howrah Duronto", "Kolkata (HWH)", "New Delhi (NDLS)", "12:40", "10:35", "21h 55m"},
        
        // Mail & Express Trains
        {"12137", "Punjab Mail", "Mumbai (CST)", "Amritsar (ASR)", "19:35", "05:05", "33h 30m"},
        {"12138", "Punjab Mail", "Amritsar (ASR)", "Mumbai (CST)", "19:35", "05:05", "33h 30m"},
        {"12723", "Telangana Express", "Hyderabad (SC)", "New Delhi (NDLS)", "06:00", "07:40", "25h 40m"},
        {"12724", "Telangana Express", "New Delhi (NDLS)", "Hyderabad (SC)", "06:00", "07:40", "25h 40m"},
        {"12861", "Coromandel Express", "Kolkata (HWH)", "Chennai (MAS)", "14:50", "17:15", "26h 25m"},
        {"12862", "Coromandel Express", "Chennai (MAS)", "Kolkata (HWH)", "14:50", "17:15", "26h 25m"},
        {"12625", "Kerala Express", "New Delhi (NDLS)", "Thiruvananthapuram (TVC)", "11:00", "13:30", "50h 30m"},
        {"12626", "Kerala Express", "Thiruvananthapuram (TVC)", "New Delhi (NDLS)", "11:00", "13:30", "50h 30m"},
        {"12903", "Golden Temple Mail", "Mumbai (CST)", "Amritsar (ASR)", "18:45", "23:35", "28h 50m"},
        {"12904", "Golden Temple Mail", "Amritsar (ASR)", "Mumbai (CST)", "18:45", "23:35", "28h 50m"},
        {"12321", "HWH-Mumbai Mail", "Kolkata (HWH)", "Mumbai (CST)", "23:35", "13:15", "37h 40m"},
        {"12322", "Mumbai-HWH Mail", "Mumbai (CST)", "Kolkata (HWH)", "23:35", "13:15", "37h 40m"},
        {"12471", "Swaraj Express", "Mumbai (CST)", "Jammu (JAT)", "11:00", "17:15", "30h 15m"},
        {"12472", "Swaraj Express", "Jammu (JAT)", "Mumbai (CST)", "11:00", "17:15", "30h 15m"},
        {"12801", "Purushottam Express", "Puri (PURI)", "New Delhi (NDLS)", "21:45", "04:00", "30h 15m"},
        {"12802", "Purushottam Express", "New Delhi (NDLS)", "Puri (PURI)", "21:45", "04:00", "30h 15m"},
        {"12779", "Goa Express", "Goa (MAO)", "New Delhi (NDLS)", "15:00", "06:30", "39h 30m"},
        {"12780", "Goa Express", "New Delhi (NDLS)", "Goa (MAO)", "15:00", "06:30", "39h 30m"},
        
        // Sampark Kranti & Other Expresses
        {"12565", "Bihar Sampark Kranti", "Patna (PNBE)", "New Delhi (NDLS)", "11:00", "05:00", "18h 0m"},
        {"12566", "Bihar Sampark Kranti", "New Delhi (NDLS)", "Patna (PNBE)", "11:00", "05:00", "18h 0m"},
        {"12393", "Sampoorna Kranti", "Patna (PNBE)", "New Delhi (NDLS)", "19:25", "07:55", "12h 30m"},
        {"12394", "Sampoorna Kranti", "New Delhi (NDLS)", "Patna (PNBE)", "19:25", "07:55", "12h 30m"},
        {"12367", "Vikramshila Express", "Patna (PNBE)", "New Delhi (NDLS)", "17:05", "12:00", "18h 55m"},
        {"12368", "Vikramshila Express", "New Delhi (NDLS)", "Patna (PNBE)", "17:05", "12:00", "18h 55m"},
        {"12715", "Sachkhand Express", "Amritsar (ASR)", "Pune (PUNE)", "05:30", "21:00", "39h 30m"},
        {"12716", "Sachkhand Express", "Pune (PUNE)", "Amritsar (ASR)", "05:30", "21:00", "39h 30m"},
        {"12451", "Shram Shakti Express", "Kanpur (CNB)", "New Delhi (NDLS)", "23:55", "05:50", "5h 55m"},
        {"12452", "Shram Shakti Express", "New Delhi (NDLS)", "Kanpur (CNB)", "23:55", "05:50", "5h 55m"},
        
        // Intercity & Shatabdi Trains
        {"12415", "Intercity Express", "New Delhi (NDLS)", "Ajmer (AII)", "06:10", "12:55", "6h 45m"},
        {"12416", "Intercity Express", "Ajmer (AII)", "New Delhi (NDLS)", "06:10", "12:55", "6h 45m"},
        {"12009", "Shatabdi Express", "Mumbai (CST)", "Ahmedabad (ADI)", "06:00", "13:00", "7h 0m"},
        {"12010", "Shatabdi Express", "Ahmedabad (ADI)", "Mumbai (CST)", "06:00", "13:00", "7h 0m"},
        {"12011", "Kalka Shatabdi", "New Delhi (NDLS)", "Chandigarh (CDG)", "07:40", "11:15", "3h 35m"},
        {"12012", "Kalka Shatabdi", "Chandigarh (CDG)", "New Delhi (NDLS)", "17:30", "21:10", "3h 40m"},
        {"12017", "Dehradun Shatabdi", "New Delhi (NDLS)", "Dehradun (DDN)", "06:50", "12:40", "5h 50m"},
        {"12018", "Dehradun Shatabdi", "Dehradun (DDN)", "New Delhi (NDLS)", "17:10", "23:05", "5h 55m"},
        {"12039", "Howrah Shatabdi", "New Delhi (NDLS)", "Kolkata (HWH)", "06:00", "17:45", "11h 45m"},
        {"12040", "Howrah Shatabdi", "Kolkata (HWH)", "New Delhi (NDLS)", "06:00", "17:45", "11h 45m"},
        
        // South Indian Routes
        {"12163", "Chennai Express", "Chennai (MAS)", "Bangalore (SBC)", "06:00", "11:30", "5h 30m"},
        {"12164", "Chennai Express", "Bangalore (SBC)", "Chennai (MAS)", "06:00", "11:30", "5h 30m"},
        {"12677", "Ernakulam Express", "Bangalore (SBC)", "Kochi (ERN)", "20:00", "07:30", "11h 30m"},
        {"12678", "Ernakulam Express", "Kochi (ERN)", "Bangalore (SBC)", "20:00", "07:30", "11h 30m"},
        {"12685", "Mangalore Express", "Chennai (MAS)", "Mangalore (MAQ)", "21:00", "13:30", "16h 30m"},
        {"12686", "Mangalore Express", "Mangalore (MAQ)", "Chennai (MAS)", "21:00", "13:30", "16h 30m"},
        {"12777", "Kochuveli Express", "Kolkata (HWH)", "Thiruvananthapuram (TVC)", "18:20", "13:30", "43h 10m"},
        {"12778", "Kochuveli Express", "Thiruvananthapuram (TVC)", "Kolkata (HWH)", "18:20", "13:30", "43h 10m"},
        
        // Western & Central Routes
        {"12955", "Jaipur Express", "Mumbai (CST)", "Jaipur (JP)", "18:30", "15:40", "21h 10m"},
        {"12956", "Jaipur Express", "Jaipur (JP)", "Mumbai (CST)", "18:30", "15:40", "21h 10m"},
        {"12961", "Avantika Express", "Indore (INDB)", "New Delhi (NDLS)", "17:25", "09:00", "15h 35m"},
        {"12962", "Avantika Express", "New Delhi (NDLS)", "Indore (INDB)", "17:25", "09:00", "15h 35m"},
        {"12979", "Jaipur Superfast", "Jaipur (JP)", "Bandra (BDTS)", "20:15", "14:00", "17h 45m"},
        {"12980", "Jaipur Superfast", "Bandra (BDTS)", "Jaipur (JP)", "20:15", "14:00", "17h 45m"},
        {"12155", "Bhopal Express", "Mumbai (CST)", "Bhopal (BPL)", "14:15", "03:45", "13h 30m"},
        {"12156", "Bhopal Express", "Bhopal (BPL)", "Mumbai (CST)", "14:15", "03:45", "13h 30m"},
        
        // Eastern Routes
        {"12987", "Sealdah Express", "Mumbai (CST)", "Kolkata (HWH)", "11:35", "11:55", "48h 20m"},
        {"12988", "Sealdah Express", "Kolkata (HWH)", "Mumbai (CST)", "11:35", "11:55", "48h 20m"},
        {"12833", "Howrah Express", "Ahmedabad (ADI)", "Kolkata (HWH)", "14:50", "06:30", "39h 40m"},
        {"12834", "Howrah Express", "Kolkata (HWH)", "Ahmedabad (ADI)", "14:50", "06:30", "39h 40m"},
        {"12841", "Coromandel Express", "Chennai (MAS)", "Kolkata (HWH)", "08:30", "11:00", "26h 30m"},
        {"12842", "Coromandel Express", "Kolkata (HWH)", "Chennai (MAS)", "08:30", "11:00", "26h 30m"},
        
        // North-East Routes
        {"12345", "Saraighat Express", "Howrah (HWH)", "Guwahati (GHY)", "15:50", "23:30", "31h 40m"},
        {"12346", "Saraighat Express", "Guwahati (GHY)", "Howrah (HWH)", "15:50", "23:30", "31h 40m"},
        {"15657", "Brahmaputra Mail", "New Delhi (NDLS)", "Guwahati (GHY)", "11:00", "20:15", "33h 15m"},
        {"15658", "Brahmaputra Mail", "Guwahati (GHY)", "New Delhi (NDLS)", "11:00", "20:15", "33h 15m"},
        
        // Punjab & J&K Routes
        {"12459", "NDLS-ASR Superfast", "New Delhi (NDLS)", "Amritsar (ASR)", "20:35", "03:30", "6h 55m"},
        {"12460", "ASR-NDLS Superfast", "Amritsar (ASR)", "New Delhi (NDLS)", "20:35", "03:30", "6h 55m"},
        {"12413", "JAT Express", "New Delhi (NDLS)", "Jammu (JAT)", "20:45", "07:25", "10h 40m"},
        {"12414", "JAT Express", "Jammu (JAT)", "New Delhi (NDLS)", "20:45", "07:25", "10h 40m"},
        
        // Popular Tourist Routes
        {"12915", "Ashram Express", "Ahmedabad (ADI)", "New Delhi (NDLS)", "21:00", "12:15", "15h 15m"},
        {"12916", "Ashram Express", "New Delhi (NDLS)", "Ahmedabad (ADI)", "21:00", "12:15", "15h 15m"},
        {"12481", "Suryanagari Express", "Jodhpur (JU)", "New Delhi (NDLS)", "17:00", "06:45", "13h 45m"},
        {"12482", "Suryanagari Express", "New Delhi (NDLS)", "Jodhpur (JU)", "17:00", "06:45", "13h 45m"},
        {"12615", "Grand Trunk Express", "Chennai (MAS)", "New Delhi (NDLS)", "19:15", "07:40", "36h 25m"},
        {"12616", "Grand Trunk Express", "New Delhi (NDLS)", "Chennai (MAS)", "19:15", "07:40", "36h 25m"}
    };
    
    // Add premium trains with fixed details
    for (const auto& td : premiumTrainData) {
        Train train(std::get<0>(td), std::get<1>(td));
        train.setFromStation(std::get<2>(td));
        train.setToStation(std::get<3>(td));
        train.setDepartureTime(std::get<4>(td));
        train.setArrivalTime(std::get<5>(td));
        train.setDuration(std::get<6>(td));
        
        // Determine train type and add appropriate classes
        std::string trainName = std::get<1>(td);
        
        if (trainName.find("Rajdhani") != std::string::npos) {
            train.addAvailability(TrainAvailability("3A", 64, 50 + (rand() % 40), 2500 + (rand() % 500)));
            train.addAvailability(TrainAvailability("2A", 48, 15 + (rand() % 25), 3500 + (rand() % 700)));
            train.addAvailability(TrainAvailability("1A", 24, 5 + (rand() % 15), 4800 + (rand() % 1000)));
        } 
        else if (trainName.find("Shatabdi") != std::string::npos) {
            train.addAvailability(TrainAvailability("CC", 80, 40 + (rand() % 50), 1500 + (rand() % 500)));
            train.addAvailability(TrainAvailability("EC", 40, 8 + (rand() % 20), 2800 + (rand() % 600)));
        }
        else if (trainName.find("Duronto") != std::string::npos) {
            train.addAvailability(TrainAvailability("SL", 200, 80 + (rand() % 100), 1000 + (rand() % 400)));
            train.addAvailability(TrainAvailability("3A", 64, 30 + (rand() % 40), 2300 + (rand() % 500)));
            train.addAvailability(TrainAvailability("2A", 48, 12 + (rand() % 25), 3200 + (rand() % 600)));
        }
        else {
            int slSeats = 100 + (rand() % 150);
            int available = slSeats - (rand() % 80);
            if (available < 0) available = -(rand() % 30);
            train.addAvailability(TrainAvailability("SL", slSeats, available, 700 + (rand() % 600)));
            train.addAvailability(TrainAvailability("3A", 64, 20 + (rand() % 50), 1800 + (rand() % 800)));
            train.addAvailability(TrainAvailability("2A", 48, 10 + (rand() % 30), 2500 + (rand() % 1000)));
            
            if (rand() % 2 == 0) {
                train.addAvailability(TrainAvailability("1A", 24, 3 + (rand() % 12), 3800 + (rand() % 1500)));
            }
        }
        
        addTrain(train);
        totalTrains++;
    }
    
    std::cout << "Total trains in system: " << trains.size() << " (including " << premiumTrainData.size() << " premium trains)" << std::endl;
}
