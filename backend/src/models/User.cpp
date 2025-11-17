#include "models/User.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <regex>

User::User() {}

User::User(const std::string& name, const std::string& email, 
           const std::string& passwordHash, const std::string& phone)
    : name(name), email(email), passwordHash(passwordHash), phone(phone) {
    
    // Generate timestamp
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%S.000Z");
    createdAt = oss.str();
    updatedAt = oss.str();
}

std::string User::getUserId() const { return userId; }
std::string User::getName() const { return name; }
std::string User::getEmail() const { return email; }
std::string User::getPasswordHash() const { return passwordHash; }
std::string User::getPhone() const { return phone; }
std::string User::getCreatedAt() const { return createdAt; }
std::string User::getUpdatedAt() const { return updatedAt; }

void User::setUserId(const std::string& id) { userId = id; }
void User::setName(const std::string& n) { 
    name = n; 
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%S.000Z");
    updatedAt = oss.str();
}

void User::setPhone(const std::string& p) { 
    phone = p; 
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%S.000Z");
    updatedAt = oss.str();
}

void User::setPasswordHash(const std::string& hash) { 
    passwordHash = hash; 
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%dT%H:%M:%S.000Z");
    updatedAt = oss.str();
}

void User::setUpdatedAt(const std::string& time) { updatedAt = time; }

bool User::isValid() const {
    // Basic validation
    if (name.empty() || email.empty() || passwordHash.empty()) {
        return false;
    }
    
    // Email validation
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        return false;
    }
    
    return true;
}

std::string User::getValidationErrors() const {
    if (name.empty()) return "Name is required";
    if (email.empty()) return "Email is required";
    if (passwordHash.empty()) return "Password is required";
    
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        return "Invalid email format";
    }
    
    return "";
}

nlohmann::json User::toJson(bool includePassword) const {
    nlohmann::json j = {
        {"userId", userId},
        {"name", name},
        {"email", email},
        {"phone", phone},
        {"createdAt", createdAt}
    };
    
    if (includePassword) {
        j["passwordHash"] = passwordHash;
    }
    
    return j;
}

User User::fromJson(const nlohmann::json& json) {
    User user;
    
    if (json.contains("userId")) user.userId = json["userId"].get<std::string>();
    if (json.contains("name")) user.name = json["name"].get<std::string>();
    if (json.contains("email")) user.email = json["email"].get<std::string>();
    if (json.contains("passwordHash")) user.passwordHash = json["passwordHash"].get<std::string>();
    if (json.contains("phone")) user.phone = json["phone"].get<std::string>();
    if (json.contains("createdAt")) user.createdAt = json["createdAt"].get<std::string>();
    if (json.contains("updatedAt")) user.updatedAt = json["updatedAt"].get<std::string>();
    
    return user;
}
