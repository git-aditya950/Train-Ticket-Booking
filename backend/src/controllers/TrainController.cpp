#include "controllers/TrainController.h"
#include "utils/DataStore.h"
#include <iostream>
#include <sstream>

TrainController::TrainController() {}

std::string TrainController::urlDecode(const std::string& str) {
    std::string result;
    char ch;
    int i, ii;
    
    for (i = 0; i < (int)str.length(); i++) {
        if (str[i] == '%') {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i = i + 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

bool TrainController::validateSearchParams(const Request& request, 
                                           std::string& from, 
                                           std::string& to,
                                           std::string& date,
                                           std::string& error) {
    from = request.getQueryParam("from");
    to = request.getQueryParam("to");
    date = request.getQueryParam("date");
    
    if (from.empty()) {
        error = "Missing required parameter: from";
        return false;
    }
    
    if (to.empty()) {
        error = "Missing required parameter: to";
        return false;
    }
    
    // URL decode
    from = urlDecode(from);
    to = urlDecode(to);
    
    return true;
}

Response TrainController::handleSearch(const Request& request) {
    Response response;
    
    std::string from, to, date, error;
    if (!validateSearchParams(request, from, to, date, error)) {
        response.setError(error, 400);
        return response;
    }
    
    std::cout << "Searching trains from '" << from << "' to '" << to << "'" << std::endl;
    
    // Search trains
    DataStore* store = DataStore::getInstance();
    std::vector<Train> trains = store->searchTrains(from, to);
    
    // Build response
    nlohmann::json trainsJson = nlohmann::json::array();
    for (const auto& train : trains) {
        trainsJson.push_back(train.toJson());
    }
    
    response.body = {
        {"status", "success"},
        {"count", trains.size()},
        {"trains", trainsJson}
    };
    
    if (trains.empty()) {
        response.body["message"] = "No trains available for this route";
    }
    
    return response;
}
