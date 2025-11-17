#include "utils/Request.h"
#include <sstream>

Request::Request() : user(nullptr) {}

Request::Request(const std::string& method, const std::string& path) 
    : method(method), path(path), user(nullptr) {}

std::string Request::getHeader(const std::string& key) const {
    auto it = headers.find(key);
    return (it != headers.end()) ? it->second : "";
}

std::string Request::getQueryParam(const std::string& key) const {
    auto it = queryParams.find(key);
    return (it != queryParams.end()) ? it->second : "";
}

std::string Request::getPathParam(const std::string& key) const {
    auto it = pathParams.find(key);
    return (it != pathParams.end()) ? it->second : "";
}

void Request::parseQueryString(const std::string& queryString) {
    if (queryString.empty()) return;
    
    std::istringstream iss(queryString);
    std::string pair;
    
    while (std::getline(iss, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            queryParams[key] = value;
        }
    }
}
