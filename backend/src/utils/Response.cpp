#include "utils/Response.h"
#include <sstream>

Response::Response(int code) : statusCode(code) {
    headers["Content-Type"] = "application/json";
}

void Response::setJson(const nlohmann::json& json) {
    body = json;
}

void Response::setError(const std::string& message, int code) {
    statusCode = code;
    body = {
        {"status", "error"},
        {"message", message}
    };
}

void Response::setSuccess(const nlohmann::json& data, const std::string& message) {
    statusCode = 200;
    body = {
        {"status", "success"},
        {"data", data}
    };
    if (!message.empty()) {
        body["message"] = message;
    }
}

std::string Response::toString() const {
    return body.dump(2);
}

std::string Response::toHttpResponse() const {
    std::ostringstream oss;
    
    // Status line
    oss << "HTTP/1.1 " << statusCode << " ";
    switch (statusCode) {
        case 200: oss << "OK"; break;
        case 201: oss << "Created"; break;
        case 400: oss << "Bad Request"; break;
        case 401: oss << "Unauthorized"; break;
        case 403: oss << "Forbidden"; break;
        case 404: oss << "Not Found"; break;
        case 500: oss << "Internal Server Error"; break;
        default: oss << "Unknown"; break;
    }
    oss << "\r\n";
    
    // Headers
    for (const auto& header : headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    
    // CORS headers
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    
    std::string bodyStr = body.dump();
    oss << "Content-Length: " << bodyStr.length() << "\r\n";
    oss << "\r\n";
    oss << bodyStr;
    
    return oss.str();
}
