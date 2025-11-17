#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>

class Request {
public:
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> pathParams;
    nlohmann::json body;
    void* user; // Will hold authenticated User pointer
    
    Request();
    Request(const std::string& method, const std::string& path);
    
    std::string getHeader(const std::string& key) const;
    std::string getQueryParam(const std::string& key) const;
    std::string getPathParam(const std::string& key) const;
    
    void parseQueryString(const std::string& queryString);
};

#endif // REQUEST_H
