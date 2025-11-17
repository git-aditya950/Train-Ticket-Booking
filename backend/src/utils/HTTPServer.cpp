#include "utils/HTTPServer.h"
#include <iostream>
#include <sstream>

HTTPServer::HTTPServer(int port, const std::string& host) 
    : port(port), host(host), router(nullptr), running(false) {
    setupCORS();
}

HTTPServer::~HTTPServer() {
    stop();
}

void HTTPServer::setRouter(Router* r) {
    router = r;
}

void HTTPServer::setupCORS() {
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
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
    
    // Setup catch-all handler
    server.set_mount_point("/", "./");
    
    // Handle all requests
    server.Get(".*", [this](const httplib::Request& req, httplib::Response& res) {
        Request request = parseHttplibRequest(req);
        Response response = router->route(request);
        
        res.status = response.statusCode;
        res.set_content(response.toString(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });
    
    server.Post(".*", [this](const httplib::Request& req, httplib::Response& res) {
        Request request = parseHttplibRequest(req);
        Response response = router->route(request);
        
        res.status = response.statusCode;
        res.set_content(response.toString(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });
    
    server.Put(".*", [this](const httplib::Request& req, httplib::Response& res) {
        Request request = parseHttplibRequest(req);
        Response response = router->route(request);
        
        res.status = response.statusCode;
        res.set_content(response.toString(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });
    
    server.Delete(".*", [this](const httplib::Request& req, httplib::Response& res) {
        Request request = parseHttplibRequest(req);
        Response response = router->route(request);
        
        res.status = response.statusCode;
        res.set_content(response.toString(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
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
