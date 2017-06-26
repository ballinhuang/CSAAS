#ifndef _SERVER
#define _SERVER

#include<iostream>
#include<ThreadPool.h>
#include<map>
#include"Monitor.hpp"
#include"Observer.hpp"
#include"s_socket.hpp"
class Server : public Observer{
    public:
        Server(Monitor *m);
        void set_server_attr(std::string,std::string);
        void set_scheduler_attr(std::string,std::string);
        void run();
        void notify();
        void attach_success();
    private:
        ThreadPool *pool;
        bool do_schedual=false;
        std::string svr_ip,svr_port,sch_ip,sch_port;
        static void f(int);
        static void x(int);
        //std::map<std::string,Node>;
};

#endif