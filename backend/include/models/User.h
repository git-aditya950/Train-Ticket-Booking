#ifndef USER_H
#define USER_H

#include <string>
#include <nlohmann/json.hpp>

class User {
private:
    std::string userId;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string phone;
    std::string createdAt;
    std::string updatedAt;

public:
    User();
    User(const std::string& name, const std::string& email, 
         const std::string& passwordHash, const std::string& phone);
    
    // Getters
    std::string getUserId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPasswordHash() const;
    std::string getPhone() const;
    std::string getCreatedAt() const;
    std::string getUpdatedAt() const;
    
    // Setters
    void setUserId(const std::string& id);
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setPasswordHash(const std::string& hash);
    void setUpdatedAt(const std::string& time);
    
    // Validation
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Serialization
    nlohmann::json toJson(bool includePassword = false) const;
    static User fromJson(const nlohmann::json& json);
};

#endif // USER_H
