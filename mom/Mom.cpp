#include "Mom.hpp"
#include <iostream>
#include "c_socket.hpp"
#include "Message.hpp"
#include "JobQueue.hpp"
#include "JobStrater.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <fstream>

using json = nlohmann::json;
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

void Mom::set_server_attr(string ip, string port)
{
    svr_ip = ip;
    svr_port = port;
}

void Mom::set_mom_attr(string ip, string port)
{
    mom_ip = ip;
    mom_port = port;
}

void Mom::notify(int msg)
{
    // now work in this version
}

void Mom::attach_success()
{
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> MOM attach_success():Attach success." << endl;
        else if (debug == 2)
            cout << "MOM ---> MOM attach_success():Attach success." << endl;
    }
}
void Mom::run()
{
    mom_socket = new s_nbsocket();
    request_pool = new ThreadPool(10);
    MomHandlerFactory *factory = new MomHandlerFactory();
    vector<Event> *events;
    if (mom_socket->setConnection(mom_ip, mom_port) != 1)
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "MOM ---> Mom run(): setConnection() Error!" << endl;
            else if (debug == 2)
                cout << "MOM ---> Mom run(): setConnection() Error!" << endl;
        }
        exit(1);
    }
    while (true)
    {
        events = mom_socket->getevent();
        for (vector<Event>::iterator it = events->begin(); it != events->end(); it++)
        {
            if (it->data == "")
                continue;
            json request = json::parse(it->data);
            //cout << it->socket_fd << " " << request["SENDER"] << " " << request["REQUEST"] << endl;
            s_socket *req_socket = new s_socket();
            req_socket->setconn_port(it->socket_fd);
            readrequest(req_socket, factory, request);
        }
    }
}
void Mom::readrequest(s_socket *s, MomHandlerFactory *factory, json request)
{
    if (request["SENDER"].get<std::string>() == "server" &&
        request["RECEIVER"].get<std::string>() == "mom" &&
        request["REQUEST"].get<std::string>() == "initmom")
    {
        if (request.count("SERVERIP") == 1 && request.count("SERVERPORT") == 1)
        {
            set_server_attr(request["SERVERIP"].get<std::string>(), request["SERVERPORT"].get<std::string>());
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> Mom run(): init success." << endl;
                else if (debug == 2)
                    cout << "MOM ---> Mom run(): init success." << endl;
            }
            return;
        }
    }
    else if (svr_ip == "" || svr_port == "")
    {
        return;
    }

    else if (request["SENDER"].get<std::string>() == "server" &&
             request["RECEIVER"].get<std::string>() == "mom" &&
             request["REQUEST"].get<std::string>() == "runjob")
    {
        signal(SIGCHLD, sig_fork);
        pid_t p;
        p = fork();
        if (p < 0)
        {
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> run(): fork() fail!" << endl;
                else if (debug == 2)
                    cout << "MOM ---> run(): fork() fail!" << endl;
            }
        }
        else if (p == 0)
        {
            mom_socket->closebind();
            JobStrater strater(request, svr_ip, svr_port);
            strater.start();
            exit(1);
        }
        else
        {
            JobQueue::GetInstance()->addjob(request["JOBID"].get<int>(), p);
        }
    }

    else
    {
        IHandler *handler = factory->getHandler(request, s);
        if (handler == NULL)
        {
            return;
        }
        handler->handle();
    }
}
void Mom::sig_fork(int signo)
{
    int stat;
    if (JobQueue::GetInstance()->i_am_jobstarter == false)
    {
        while (waitpid(-1, &stat, WNOHANG) > 0)
            ;
        JobQueue::GetInstance()->erasedownjob();
        //cout << "some job down" << endl;
    }
    else
        return;
    return;
}

/*
void Mom::readrequest(s_socket *s, MomHandlerFactory *factory, json request)
{
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> Mom readrequest():Start read request." << endl;
        else if (debug == 2)
            cout << "MOM ---> Mom readrequest():Start read request." << endl;
    }

    IHandler *handler = factory->getHandler(request, s);
    if (handler == NULL)
    {
        s->closeConnection();
        return;
    }
    handler->handle();
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> Mom readrequest():End read request." << endl;
        else if (debug == 2)
            cout << "MOM ---> Mom readrequest():End read request." << endl;
    }
    s->closeConnection();
}

void Mom::run()
{
    // init mom
    while (svr_ip == "" && svr_port == "")
    {
        s_socket *initsocket = new s_socket();
        if (initsocket->setConnection(mom_ip, mom_port) != 1)
        {
            cerr << "MOM ---> Mom run(): socket bind error!" << endl;
            exit(1);
        }
        if (initsocket->acceptClinet())
        {
            initsocket->setacceptreuse();
            initsocket->closebind();
            string res = "";
            res = initsocket->readmessage();
            if (res == "")
            {
                initsocket->closeConnection();
                continue;
            }
            json request = json::parse(res);
            if (request["SENDER"].get<std::string>() == "server" &&
                request["RECEIVER"].get<std::string>() == "mom" &&
                request["REQUEST"].get<std::string>() == "initmom")
            {
                if (request.count("SERVERIP") == 1 && request.count("SERVERPORT") == 1)
                {
                    set_server_attr(request["SERVERIP"].get<std::string>(), request["SERVERPORT"].get<std::string>());
                    initsocket->closeConnection();
                    if (debug > 0)
                    {
                        if (debug == 1)
                            *debug_file << "MOM ---> Mom run(): init success." << endl;
                        else if (debug == 2)
                            cout << "MOM ---> Mom run(): init success." << endl;
                    }
                    break;
                }
            }
        }
        initsocket->closeConnection();
    }
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> Mom run(): init complete Start." << endl;
        else if (debug == 2)
            cout << "MOM ---> Mom run():  init complete Start." << endl;
    }
    request_pool = new ThreadPool(10);
    MomHandlerFactory *factory = new MomHandlerFactory();
    while (1)
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "MOM ---> Mom run(): Creat new socket." << endl;
            else if (debug == 2)
                cout << "MOM ---> Mom run(): Creat new socket." << endl;
        }
        s_socket *s = new s_socket();
        if (s->setConnection(mom_ip, mom_port) != 1)
        {
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> Mom run(): setConnection() Error!" << endl;
                else if (debug == 2)
                    cout << "MOM ---> Mom run(): setConnection() Error!" << endl;
            }
            continue;
        }
        if (s->acceptClinet())
        {
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> Mom run(): Accept client success." << endl;
                else if (debug == 2)
                    cout << "MOM ---> Mom run(): Accept client success." << endl;
            }
            s->setacceptreuse();
            s->closebind();
            string res = "";
            res = s->readmessage();
            if (res == "")
            {
                s->closeConnection();
                continue;
            }
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> Mom run():Receive request = " << endl
                                << res << endl;
                else if (debug == 2)
                    cout << "MOM ---> Mom run():Receive request = " << endl
                         << res << endl;
            }
            json request = json::parse(res);
            if (request["SENDER"].get<std::string>() == "server" &&
                request["RECEIVER"].get<std::string>() == "mom" &&
                request["REQUEST"].get<std::string>() == "runjob")
            {
                signal(SIGCHLD, sig_fork);
                pid_t p;
                p = fork();
                if (p < 0)
                {
                    if (debug > 0)
                    {
                        if (debug == 1)
                            *debug_file << "MOM ---> run(): fork() fail!" << endl;
                        else if (debug == 2)
                            cout << "MOM ---> run(): fork() fail!" << endl;
                    }
                }
                else if (p == 0)
                {
                    JobStrater strater(request, svr_ip, svr_port);
                    strater.start();
                    exit(1);
                }
                else
                {
                    s->closeConnection();
                    JobQueue::GetInstance()->addjob(request["JOBID"].get<int>(), p);
                    continue;
                }
            }
            else
            {
                request_pool->enqueue(&readrequest, s, factory, request);
            }
        }
    }
}
*/