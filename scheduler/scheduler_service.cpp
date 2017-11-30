#include "scheduler_service.hpp"
#include "cc_socket.hpp"
#include <vector>
#include <dlfcn.h>

scheduler_service::scheduler_service(string mode, string ip, string port)
{
    if(debug == 2)
        cout << "Before set mode." << endl;
    set_mode(mode);
    server_ip = ip;
    server_port = port;
}

void scheduler_service::do_schedule(string server_ip, string server_port)
{
    cc_socket *socket = new cc_socket();
    if (socket->setConnection(server_ip, server_port) == 0)
    {
        return;
    }
    if (socket->connect2server() == 0)
    {
        return;
    }
    IScheHandler *ScheHandler = factory->getScheHandler(scheduler, socket);
    if (ScheHandler == NULL)
        return;
    if(debug == 2)
        cout << "Before handleschedule." << endl;
    ScheHandler->handleschedule();
    socket->closeConnection();
}

void scheduler_service::startservice()
{
    factory = new ScheHandlerFactory();
    s_nbsocket *sch_socket = new s_nbsocket();
    if (sch_socket->setConnection(server_ip, server_port) != 1)
    {
        exit(1);
    }
    vector<Event> *events;
    while (true)
    {
        events = sch_socket->getevent();
        for (vector<Event>::iterator it = events->begin(); it != events->end(); it++)
        {
            if (it->data == "")
                continue;
            json request = json::parse(it->data);
            handlerequest(request);
        }
    }
}

void scheduler_service::handlerequest(json request)
{
    //do_schedule
    if (request["SENDER"].get<std::string>() == "server" &&
        request["RECEIVER"].get<std::string>() == "scheduler" &&
        request["REQUEST"].get<std::string>() == "do_schedule")
    {
        string server_ip = "", server_port = "";
        server_ip = request["IP"].get<std::string>();
        server_port = request["PORT"].get<std::string>();
        if (server_ip == "" || server_port == "")
        {
            return;
        }
        if(debug == 2)
            cout << "Before do_schedule." << endl;
        do_schedule(server_ip, server_port);
    }
    else if (request["SENDER"].get<std::string>() == "server" &&
             request["RECEIVER"].get<std::string>() == "scheduler" &&
             request["REQUEST"].get<std::string>() == "change_mode")
    {
        //change schedule mode
        string mode = request["MODE"].get<std::string>();
        set_mode(mode);
        cout << "change " << mode << endl;
    }
}

void scheduler_service::set_mode(string mode)
{
    schedule_mode = mode;
    if (mode == "")
    {
        scheduler = NULL;
        return;
    }

    IScheduler* (*getInstance)();

    handle = dlopen(mode.c_str(), RTLD_LAZY);
    if(!handle) {
        cout << "dlopen handle error!" << endl;
        exit(1);
    }

    getInstance = (IScheduler* (*)())dlsym(handle, "getInstance");
    if(dlerror() != NULL) {
        cout << "getInstance function loaded error!" << endl;
        exit(1);
    }
    scheduler = getInstance();
}