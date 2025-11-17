#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <httplib.h>
#include "utils/Router.h"

class HTTPServer {
private:
    int port;
    std::string host;
    Router* router;
    httplib::Server server;
    bool running;

public:
    HTTPServer(int port = 18080, const std::string& host = "localhost");
    ~HTTPServer();
    
    void setRouter(Router* router);
    void start();
    void stop();
    bool isRunning() const;
    
private:
    void setupCORS();
    Request parseHttplibRequest(const httplib::Request& req);
};

#endif // HTTPSERVER_H
