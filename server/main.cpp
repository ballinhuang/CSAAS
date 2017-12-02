#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <s_socket.hpp>
#include "Server.hpp"
#include "Monitor.hpp"
#include <mutex>
#include <ctime>
using namespace std;

mutex monitor_mtx;
Monitor *Monitor::monitor = 0;
int debug = 0;
ofstream *debug_file;
fstream logFile;

void setLogName() {
    time_t t = time(0);
    char timer[99];
    string logName = "CrownLog";

    strftime(timer, sizeof(timer), "%Y%m%d%H%M%S", localtime(&t));
    logName += string(timer) + ".log";
    
    logFile.open(logName.c_str(), ios::out);
    if(!logFile.is_open()) {
        cout << "Generation of log file is error!" << endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{

    string server_ip = "", server_port = "";
    string scheduler_ip = "", scheduler_port = "";
    if (argc > 1)
    {
        string arg;
        for (int i = 1; i < argc; i++)
        {
            arg = std::string(argv[i]);
            if (arg == "-fdebug")
            {
                debug = 1;
                debug_file = new ofstream();
                debug_file->open("debug.txt");
            }
            else if (arg == "-odebug")
            {
                debug = 2;
            }
            else if (arg == "-i")
            {
                i++;
                server_ip = std::string(argv[i]);
            }
            else if (arg == "-p")
            {
                i++;
                server_port = std::string(argv[i]);
            }
            else if (arg == "-si")
            {
                i++;
                scheduler_ip = std::string(argv[i]);
            }
            else if (arg == "-sp")
            {
                i++;
                scheduler_port = std::string(argv[i]);
            }
            else
            {
                cout << "Server ---> main(): Error command (" << arg << ") !" << endl;
                exit(1);
            }
        }
    }
    if ((server_ip == "" && server_port != "") || (server_ip != "" && server_port == ""))
    {
        cout << "Server ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }
    if ((scheduler_ip == "" && scheduler_port != "") || (scheduler_ip != "" && scheduler_port == ""))
    {
        cout << "Server ---> main(): Error! -si -sp must be used at the same time." << endl;
        exit(1);
    }

    ifstream f;
    if (server_ip == "" && server_port == "")
    {
        f.open("server.con");
        if (f.is_open())
        {
            f >> server_ip;
            f >> server_port;
        }
        else
        {
            cout << "Server ---> main(): Error! server.con not found." << endl;
            exit(1);
        }
        f.close();
    }
    if (scheduler_ip == "" && scheduler_port == "")
    {
        f.open("scheduler.con");

        if (f.is_open())
        {
            f >> scheduler_ip;
            f >> scheduler_port;
        }
        else
        {
            cout << "Server ---> main(): Error! scheduler.con not found." << endl;
            exit(1);
        }
        f.close();
    }
    // Test socket port
    s_socket s;
    if (s.setConnection(server_ip, server_port) == 0)
    {
        cout << "-1";
        s.closebind();
        return 0;
    }
    s.closebind();
    //creat Server and Monitor , attach Server to Monitor.

    setLogName();
    Server *server = new Server();
    Monitor::GetInstance()->attachserver(server);
    server->set_server_attr(server_ip, server_port);
    server->set_scheduler_attr(scheduler_ip, scheduler_port);
    Monitor::GetInstance()->setnodelist();
    Monitor::GetInstance()->sch_ip = scheduler_ip;
    Monitor::GetInstance()->sch_port = scheduler_port;
    Monitor::GetInstance()->setstarttime();
    pid_t pid = fork();
    if (pid < 0)
    {
        cout << "Server ---> main(): Fork Error !" << endl;
        exit(1);
    }
    else if (pid != 0)
    {
        cout << pid;
        return 0;
    }
    server->run();
}