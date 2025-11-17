#include "controllers/AuthController.h"
#include "models/User.h"
#include <iostream>

AuthController::AuthController() {}

bool AuthController::validateRegisterInput(const nlohmann::json& data, std::string& error) {
    if (!data.contains("name") || data["name"].get<std::string>().empty()) {
        error = "Name is required";
        return false;
    }
    if (!data.contains("email") || data["email"].get<std::string>().empty()) {
        error = "Email is required";
        return false;
    }
    if (!data.contains("password") || data["password"].get<std::string>().empty()) {
        error = "Password is required";
        return false;
    }
    if (data["password"].get<std::string>().length() < 8) {
        error = "Password must be at least 8 characters";
        return false;
    }
    return true;
}

bool AuthController::validateLoginInput(const nlohmann::json& data, std::string& error) {
    if (!data.contains("email") || data["email"].get<std::string>().empty()) {
        error = "Email is required";
        return false;
    }
    if (!data.contains("password") || data["password"].get<std::string>().empty()) {
        error = "Password is required";
        return false;
    }
    return true;
}

Response AuthController::handleRegister(const Request& request) {
    Response response;
    
    // Validate input
    std::string error;
    if (!validateRegisterInput(request.body, error)) {
        response.setError(error, 400);
        return response;
    }
    
    std::string name = request.body["name"].get<std::string>();
    std::string email = request.body["email"].get<std::string>();
    std::string password = request.body["password"].get<std::string>();
    std::string phone = request.body.contains("phone") ? 
                       request.body["phone"].get<std::string>() : "";
    
    // Register user
    User* user = authService.registerUser(name, email, password, phone);
    
    if (!user) {
        response.setError("Email already exists or registration failed", 400);
        return response;
    }
    
    // Generate token
    auto loginResult = authService.loginUser(email, password);
    std::string token = loginResult.second;
    
    // Return success
    response.statusCode = 201;
    response.body = {
        {"status", "success"},
        {"message", "User registered successfully"},
        {"data", user->toJson()},
        {"token", token}
    };
    
    return response;
}

Response AuthController::handleLogin(const Request& request) {
    Response response;
    
    // Validate input
    std::string error;
    if (!validateLoginInput(request.body, error)) {
        response.setError(error, 400);
        return response;
    }
    
    std::string email = request.body["email"].get<std::string>();
    std::string password = request.body["password"].get<std::string>();
    
    // Login user
    auto result = authService.loginUser(email, password);
    User* user = result.first;
    std::string token = result.second;
    
    if (!user) {
        response.setError("Invalid email or password", 401);
        return response;
    }
    
    // Return success
    response.body = {
        {"status", "success"},
        {"message", "Login successful"},
        {"data", user->toJson()},
        {"token", token}
    };
    
    return response;
}

Response AuthController::handleLogout(const Request& request) {
    Response response;
    
    std::string authHeader = request.getHeader("Authorization");
    if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
        response.setError("Missing authorization header", 401);
        return response;
    }
    
    std::string token = authHeader.substr(7);
    authService.logoutUser(token);
    
    response.body = {
        {"status", "success"},
        {"message", "Logged out successfully"}
    };
    
    return response;
}

Response AuthController::handleGetProfile(const Request& request) {
    Response response;
    
    User* user = static_cast<User*>(request.user);
    if (!user) {
        response.setError("User not authenticated", 401);
        return response;
    }
    
    response.setSuccess(user->toJson());
    return response;
}

Response AuthController::handleUpdateProfile(const Request& request) {
    Response response;
    
    User* user = static_cast<User*>(request.user);
    if (!user) {
        response.setError("User not authenticated", 401);
        return response;
    }
    
    // Update profile
    if (!authService.updateProfile(user, request.body)) {
        response.setError("Failed to update profile. Check current password.", 400);
        return response;
    }
    
    response.body = {
        {"status", "success"},
        {"message", "Profile updated successfully"},
        {"data", user->toJson()}
    };
    
    return response;
}
