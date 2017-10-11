#include "HandlerFactory.hpp"
#include <iostream>
#include <fstream>
using namespace std;

extern int debug;
extern ofstream *debug_file;

IHandler *HandlerFactory::getHandler(json request, s_socket *s)
{

    //NewJobHandler
    if (request["SENDER"].get<std::string>() == "subjob" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "newjob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return NewJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return NewJobHandler." << endl;
        }
        return new NewJobHandler(request, s);
    }
    //NewJobHandler

    //RunJobHandler
    if (request["SENDER"].get<std::string>() == "scheduler" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "runjob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return RunJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return RunJobHandler." << endl;
        }
        return new RunJobHandler(request, s);
    }
    //RunJobHandler

    //QueueStateHandler
    if (request["SENDER"].get<std::string>() == "scheduler" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "queuestate")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return QueueStateHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return QueueStateHandler." << endl;
        }
        return new QueueStateHandler(request, s);
    }
    //QueueStateHandler

    //NodeStateHandler
    if (request["SENDER"].get<std::string>() == "scheduler" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "nodestate")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return NodeStateHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return NodeStateHandler." << endl;
        }
        return new NodeStateHandler(request, s);
    }
    //NodeStateHandler

    //DoneJobHandler
    if (request["SENDER"].get<std::string>() == "mom" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "donejob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return DoneJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return DoneJobHandler." << endl;
        }
        return new DoneJobHandler(request, s);
    }
    //DoneJobHandler

    //FailJobHandler
    if (request["SENDER"].get<std::string>() == "mom" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "failjob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return FailJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return FailJobHandler." << endl;
        }
        return new FailJobHandler(request, s);
    }
    //FailJobHandler

    //JobStateHandler
    if (request["SENDER"].get<std::string>() == "jobstate" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "jobliststate")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return JobStateHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return JobStateHandler." << endl;
        }
        return new JobStateHandler(request, s);
    }
    //JobStateHandler

    //KillJobHandler
    if (request["SENDER"].get<std::string>() == "killjob" &&
        request["RECEIVER"].get<std::string>() == "server" &&
        request["REQUEST"].get<std::string>() == "killjob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return KillJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return KillJobHandler." << endl;
        }
        return new KillJobHandler(request, s);
    }
    //KillJobHandler

    //ERROR
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "HandlerFactory getHandler(): ERROR!!"
                        << "----> ERROR request = " << endl
                        << request.dump() << endl;
        else if (debug == 2)
            cout << "HandlerFactory getHandler(): ERROR!!"
                 << "----> ERROR request = " << endl
                 << request.dump() << endl;
    }

    return NULL;
}