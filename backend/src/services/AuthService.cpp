#include "services/AuthService.h"
#include "utils/DataStore.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <openssl/sha.h>

AuthService::AuthService() {}

std::string AuthService::hashPassword(const std::string& password) {
    // Simple SHA256 hash (in production, use bcrypt or argon2)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return oss.str();
}

bool AuthService::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}

std::string AuthService::generateUserId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    std::ostringstream oss;
    oss << "user_" << dis(gen);
    return oss.str();
}

std::string AuthService::generateToken(const User& user) {
    // Simple token generation (in production, use JWT)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    const char* hexChars = "0123456789abcdef";
    std::ostringstream oss;
    oss << "token_";
    
    for (int i = 0; i < 32; i++) {
        oss << hexChars[dis(gen)];
    }
    
    // Add timestamp
    oss << "_" << std::time(nullptr);
    
    return oss.str();
}

User* AuthService::registerUser(const std::string& name, 
                                const std::string& email,
                                const std::string& password, 
                                const std::string& phone) {
    DataStore* store = DataStore::getInstance();
    
    // Check if email already exists
    User* existingUser = store->findUserByEmail(email);
    if (existingUser) {
        return nullptr; // Email already exists
    }
    
    // Validate password length
    if (password.length() < 8) {
        return nullptr; // Password too short
    }
    
    // Hash password
    std::string passwordHash = hashPassword(password);
    
    // Create user
    User user(name, email, passwordHash, phone);
    user.setUserId(generateUserId());
    
    // Validate user
    if (!user.isValid()) {
        return nullptr;
    }
    
    // Store user
    if (!store->addUser(user)) {
        return nullptr;
    }
    
    return store->findUserById(user.getUserId());
}

std::pair<User*, std::string> AuthService::loginUser(const std::string& email, 
                                                     const std::string& password) {
    DataStore* store = DataStore::getInstance();
    
    // Find user
    User* user = store->findUserByEmail(email);
    if (!user) {
        return {nullptr, ""};
    }
    
    // Verify password
    if (!verifyPassword(password, user->getPasswordHash())) {
        return {nullptr, ""};
    }
    
    // Generate token
    std::string token = generateToken(*user);
    
    // Store session
    store->addSession(token, user->getUserId());
    
    return {user, token};
}

bool AuthService::logoutUser(const std::string& token) {
    DataStore* store = DataStore::getInstance();
    store->removeSession(token);
    return true;
}

User* AuthService::validateToken(const std::string& token) {
    DataStore* store = DataStore::getInstance();
    
    // Get userId from token
    std::string userId = store->getUserIdFromToken(token);
    if (userId.empty()) {
        return nullptr;
    }
    
    // Find user
    return store->findUserById(userId);
}

bool AuthService::updateProfile(User* user, const nlohmann::json& updates) {
    if (!user) return false;
    
    DataStore* store = DataStore::getInstance();
    
    // Update name
    if (updates.contains("name")) {
        user->setName(updates["name"].get<std::string>());
    }
    
    // Update phone
    if (updates.contains("phone")) {
        user->setPhone(updates["phone"].get<std::string>());
    }
    
    // Update password
    if (updates.contains("newPassword") && updates.contains("currentPassword")) {
        std::string currentPassword = updates["currentPassword"].get<std::string>();
        std::string newPassword = updates["newPassword"].get<std::string>();
        
        // Verify current password
        if (!verifyPassword(currentPassword, user->getPasswordHash())) {
            return false;
        }
        
        // Validate new password length
        if (newPassword.length() < 8) {
            return false;
        }
        
        // Hash and update password
        user->setPasswordHash(hashPassword(newPassword));
    }
    
    // Save changes
    return store->updateUser(*user);
}
