#ifndef _MOM
#define _MOM

#include <iostream>
#include <ThreadPool.h>
#include <map>
#include <mutex>
#include "Observer.hpp"
#include "s_socket.hpp"
#include "MomHandlerFactory.hpp"


class Mom : public Observer{
    public:
        void set_mom_attr(std::string,std::string);
        void set_server_attr(std::string,std::string);
        void run();
        void notify(int);
        void attach_success();
        int contact_server();
    private:
        static void start_accept_thread(std::string,std::string,ThreadPool*);
        static void readrequest(s_socket*,MomHandlerFactory*);
        ThreadPool *request_pool;
        ThreadPool *mom_pool;
        std::string svr_ip,svr_port,mom_ip,mom_port;
        bool job_down = false;
        std::mutex job_down_tex;
};

#endif