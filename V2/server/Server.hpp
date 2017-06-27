#ifndef _SERVER
#define _SERVER

#include<iostream>
#include<ThreadPool.h>
#include<map>
#include<mutex>
#include"Monitor.hpp"
#include"Observer.hpp"
#include"s_socket.hpp"
#include"HandlerFactory.hpp"
class Server : public Observer{
    public:
        Server(Monitor *m);
        void set_server_attr(std::string,std::string);
        void set_scheduler_attr(std::string,std::string);
        void run();
        void notify();
        void attach_success();
    private:
        static void start_accept_thread(std::string,std::string,ThreadPool*);
        static void readrequest(s_socket*,HandlerFactory*);
        ThreadPool *request_pool;
        ThreadPool *server_pool;
        bool do_schedual=false;
        std::mutex do_schedual_tex;
        std::string svr_ip,svr_port,sch_ip,sch_port;
        //std::map<std::string,Node>;
};

#endif