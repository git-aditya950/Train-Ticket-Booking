#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>

class Response {
public:
    int statusCode;
    std::map<std::string, std::string> headers;
    nlohmann::json body;
    
    Response(int code = 200);
    
    void setJson(const nlohmann::json& json);
    void setError(const std::string& message, int code = 400);
    void setSuccess(const nlohmann::json& data, const std::string& message = "");
    std::string toString() const;
    std::string toHttpResponse() const;
};

#endif // RESPONSE_H
