#include "utils/Router.h"
#include "services/AuthService.h"
#include <iostream>

Router::Router() {}

void Router::addRoute(const std::string& method, 
                     const std::string& path,
                     std::function<Response(const Request&)> handler,
                     bool requiresAuth) {
    Route route;
    route.method = method;
    route.pattern = path;
    route.handler = handler;
    route.requiresAuth = requiresAuth;
    
    // Convert path pattern to regex
    std::string regexPattern = path;
    // Replace :param with ([^/]+) for capturing
    size_t pos = 0;
    while ((pos = regexPattern.find(":", pos)) != std::string::npos) {
        size_t end = regexPattern.find("/", pos);
        if (end == std::string::npos) {
            end = regexPattern.length();
        }
        regexPattern.replace(pos, end - pos, "([^/]+)");
        pos = end;
    }
    
    route.pathRegex = std::regex(regexPattern);
    routes.push_back(route);
}

Response Router::route(Request& request) {
    // Handle OPTIONS (preflight)
    if (request.method == "OPTIONS") {
        Response response(204);
        return response;
    }
    
    // Find matching route
    for (const auto& route : routes) {
        if (route.method != request.method) continue;
        
        std::smatch matches;
        if (std::regex_match(request.path, matches, route.pathRegex)) {
            // Extract path parameters
            std::string pattern = route.pattern;
            size_t paramIndex = 1;
            size_t pos = 0;
            while ((pos = pattern.find(":", pos)) != std::string::npos) {
                size_t end = pattern.find("/", pos);
                if (end == std::string::npos) {
                    end = pattern.length();
                }
                std::string paramName = pattern.substr(pos + 1, end - pos - 1);
                if (paramIndex < matches.size()) {
                    request.pathParams[paramName] = matches[paramIndex].str();
                    paramIndex++;
                }
                pos = end;
            }
            
            // Check authentication if required
            if (route.requiresAuth) {
                std::string authHeader = request.getHeader("Authorization");
                if (authHeader.empty() || authHeader.substr(0, 7) != "Bearer ") {
                    Response response;
                    response.setError("Missing or invalid authorization header", 401);
                    return response;
                }
                
                std::string token = authHeader.substr(7);
                AuthService authService;
                void* user = authService.validateToken(token);
                
                if (!user) {
                    Response response;
                    response.setError("Invalid or expired token", 401);
                    return response;
                }
                
                request.user = user;
            }
            
            // Call handler
            return route.handler(request);
        }
    }
    
    // No route found
    Response response;
    response.setError("Route not found", 404);
    return response;
}
