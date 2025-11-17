#include "utils/HTTPServer.h"
#include <iostream>
#include <sstream>

HTTPServer::HTTPServer(int port, const std::string& host) 
    : port(port), host(host), router(nullptr), running(false) {
    // CORS will be set up in start() method
}

HTTPServer::~HTTPServer() {
    stop();
}

void HTTPServer::setRouter(Router* r) {
    router = r;
}

void HTTPServer::setupCORS() {
    // Set up preflight handler with wildcard pattern
    server.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Access-Control-Max-Age", "86400");
        res.status = 204;
    });
}

Request HTTPServer::parseHttplibRequest(const httplib::Request& req) {
    Request request(req.method, req.path);
    
    // Parse headers
    for (const auto& header : req.headers) {
        request.headers[header.first] = header.second;
    }
    
    // Parse query parameters
    for (const auto& param : req.params) {
        request.queryParams[param.first] = param.second;
    }
    
    // Parse body if JSON
    if (!req.body.empty()) {
        try {
            request.body = nlohmann::json::parse(req.body);
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse JSON body: " << e.what() << std::endl;
        }
    }
    
    return request;
}

void HTTPServer::start() {
    if (!router) {
        std::cerr << "Error: Router not set!" << std::endl;
        return;
    }
    
    running = true;
    
    // Setup CORS preflight handler
    setupCORS();
    
    // Helper lambda to add CORS headers to responses
    auto addCORSHeaders = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    };
    
    // Handle all requests
    server.Get(R"(.*)", [this, addCORSHeaders](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "GET " << req.path << std::endl;
            Request request = parseHttplibRequest(req);
            Response response = router->route(request);
            
            res.status = response.statusCode;
            res.set_content(response.toString(), "application/json");
            addCORSHeaders(res);
        } catch (const std::exception& e) {
            std::cerr << "Error handling GET request: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"status\":\"error\",\"message\":\"Internal server error\"}", "application/json");
            addCORSHeaders(res);
        }
    });
    
    server.Post(R"(.*)", [this, addCORSHeaders](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "POST " << req.path << std::endl;
            Request request = parseHttplibRequest(req);
            Response response = router->route(request);
            
            res.status = response.statusCode;
            res.set_content(response.toString(), "application/json");
            addCORSHeaders(res);
        } catch (const std::exception& e) {
            std::cerr << "Error handling POST request: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"status\":\"error\",\"message\":\"Internal server error\"}", "application/json");
            addCORSHeaders(res);
        }
    });
    
    server.Put(R"(.*)", [this, addCORSHeaders](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "PUT " << req.path << std::endl;
            Request request = parseHttplibRequest(req);
            Response response = router->route(request);
            
            res.status = response.statusCode;
            res.set_content(response.toString(), "application/json");
            addCORSHeaders(res);
        } catch (const std::exception& e) {
            std::cerr << "Error handling PUT request: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"status\":\"error\",\"message\":\"Internal server error\"}", "application/json");
            addCORSHeaders(res);
        }
    });
    
    server.Delete(R"(.*)", [this, addCORSHeaders](const httplib::Request& req, httplib::Response& res) {
        try {
            std::cout << "DELETE " << req.path << std::endl;
            Request request = parseHttplibRequest(req);
            Response response = router->route(request);
            
            res.status = response.statusCode;
            res.set_content(response.toString(), "application/json");
            addCORSHeaders(res);
        } catch (const std::exception& e) {
            std::cerr << "Error handling DELETE request: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"status\":\"error\",\"message\":\"Internal server error\"}", "application/json");
            addCORSHeaders(res);
        }
    });
    
    std::cout << "TrainTrack Backend Server starting on http://" << host << ":" << port << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    server.listen(host.c_str(), port);
}

void HTTPServer::stop() {
    if (running) {
        server.stop();
        running = false;
        std::cout << "Server stopped" << std::endl;
    }
}

bool HTTPServer::isRunning() const {
    return running;
}
