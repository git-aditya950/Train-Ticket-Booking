#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <string>
#include <nlohmann/json.hpp>
#include "models/User.h"

class AuthService {
public:
    AuthService();
    
    // Authentication Logic
    User* registerUser(const std::string& name, 
                      const std::string& email,
                      const std::string& password, 
                      const std::string& phone);
    
    std::pair<User*, std::string> loginUser(const std::string& email, 
                                           const std::string& password);
    
    bool logoutUser(const std::string& token);
    
    User* validateToken(const std::string& token);
    
    bool updateProfile(User* user, const nlohmann::json& updates);
    
private:
    // Security
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
    std::string generateToken(const User& user);
    std::string generateUserId();
};

#endif // AUTHSERVICE_H
