#include <iostream>
#include <csignal>
#include "utils/HTTPServer.h"
#include "utils/Router.h"
#include "utils/DataStore.h"
#include "controllers/AuthController.h"
#include "controllers/TrainController.h"
#include "controllers/BookingController.h"

HTTPServer* serverPtr = nullptr;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    
    if (serverPtr) {
        serverPtr->stop();
    }
    
    exit(signum);
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  TrainTrack Backend Server" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Initialize data store
    DataStore* store = DataStore::getInstance();
    store->initializeSampleData();
    
    // Create controllers
    AuthController authController;
    TrainController trainController;
    BookingController bookingController;
    
    // Create router
    Router router;
    
    // Auth routes
    router.addRoute("POST", "/api/auth/register", 
        [&authController](const Request& req) { 
            return authController.handleRegister(req); 
        });
    
    router.addRoute("POST", "/api/auth/login", 
        [&authController](const Request& req) { 
            return authController.handleLogin(req); 
        });
    
    router.addRoute("POST", "/api/auth/logout", 
        [&authController](const Request& req) { 
            return authController.handleLogout(req); 
        }, true);
    
    router.addRoute("GET", "/api/auth/profile", 
        [&authController](const Request& req) { 
            return authController.handleGetProfile(req); 
        }, true);
    
    router.addRoute("PUT", "/api/auth/profile", 
        [&authController](const Request& req) { 
            return authController.handleUpdateProfile(req); 
        }, true);
    
    // Train routes
    router.addRoute("GET", "/api/search", 
        [&trainController](const Request& req) { 
            return trainController.handleSearch(req); 
        });
    
    // Booking routes
    router.addRoute("POST", "/api/bookings", 
        [&bookingController](const Request& req) { 
            return bookingController.handleCreateBooking(req); 
        }, true);
    
    router.addRoute("GET", "/api/bookings", 
        [&bookingController](const Request& req) { 
            return bookingController.handleGetBookings(req); 
        }, true);
    
    router.addRoute("GET", "/api/bookings/:bookingId", 
        [&bookingController](const Request& req) { 
            return bookingController.handleGetBookingById(req); 
        }, true);
    
    router.addRoute("DELETE", "/api/bookings/:bookingId", 
        [&bookingController](const Request& req) { 
            return bookingController.handleCancelBooking(req); 
        }, true);
    
    // Create server
    HTTPServer server(18080, "0.0.0.0");
    server.setRouter(&router);
    serverPtr = &server;
    
    // Register signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Start server
    std::cout << "\nServer configuration:" << std::endl;
    std::cout << "  Host: 0.0.0.0" << std::endl;
    std::cout << "  Port: 18080" << std::endl;
    std::cout << "  Base URL: http://localhost:18080/api" << std::endl;
    std::cout << "\nAvailable endpoints:" << std::endl;
    std::cout << "  POST   /api/auth/register" << std::endl;
    std::cout << "  POST   /api/auth/login" << std::endl;
    std::cout << "  POST   /api/auth/logout" << std::endl;
    std::cout << "  GET    /api/auth/profile" << std::endl;
    std::cout << "  PUT    /api/auth/profile" << std::endl;
    std::cout << "  GET    /api/search" << std::endl;
    std::cout << "  POST   /api/bookings" << std::endl;
    std::cout << "  GET    /api/bookings" << std::endl;
    std::cout << "  GET    /api/bookings/:bookingId" << std::endl;
    std::cout << "  DELETE /api/bookings/:bookingId" << std::endl;
    std::cout << "\n==================================" << std::endl;
    
    server.start();
    
    return 0;
}
