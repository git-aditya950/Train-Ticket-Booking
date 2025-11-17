#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <regex>
#include "utils/Request.h"
#include "utils/Response.h"

struct Route {
    std::string method;
    std::string pattern;
    std::regex pathRegex;
    std::function<Response(const Request&)> handler;
    bool requiresAuth;
};

class Router {
private:
    std::vector<Route> routes;
    
    bool matchRoute(const std::string& pattern, const std::string& path,
                   std::map<std::string, std::string>& params);

public:
    Router();
    
    void addRoute(const std::string& method, 
                 const std::string& path,
                 std::function<Response(const Request&)> handler,
                 bool requiresAuth = false);
    
    Response route(Request& request);
};

#endif // ROUTER_H
