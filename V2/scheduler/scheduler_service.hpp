#ifndef _SCHEDULER_SERVICE
#define _SCHEDULER_SERVICE
#include "s_socket.hpp"
#include <iostream>


class scheduler_service{
    public:
    scheduler_service(std::string,std::string,std::string);
    int do_schedule();
    private:
    std::string schedule_mode,server_ip,server_port;
};

#endif