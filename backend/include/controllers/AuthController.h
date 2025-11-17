#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include "utils/Request.h"
#include "utils/Response.h"
#include "services/AuthService.h"

class AuthController {
private:
    AuthService authService;
    
    bool validateRegisterInput(const nlohmann::json& data, std::string& error);
    bool validateLoginInput(const nlohmann::json& data, std::string& error);

public:
    AuthController();
    
    Response handleRegister(const Request& request);
    Response handleLogin(const Request& request);
    Response handleLogout(const Request& request);
    Response handleGetProfile(const Request& request);
    Response handleUpdateProfile(const Request& request);
};

#endif // AUTHCONTROLLER_H
