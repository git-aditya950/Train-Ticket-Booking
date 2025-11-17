#ifndef TRAINCONTROLLER_H
#define TRAINCONTROLLER_H

#include "utils/Request.h"
#include "utils/Response.h"

class TrainController {
private:
    bool validateSearchParams(const Request& request, 
                             std::string& from, 
                             std::string& to,
                             std::string& date,
                             std::string& error);
    
    std::string urlDecode(const std::string& str);

public:
    TrainController();
    
    Response handleSearch(const Request& request);
};

#endif // TRAINCONTROLLER_H
