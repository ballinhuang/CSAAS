#include <json.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "s_socket.hpp"
#include "scheduler_service.hpp"

using namespace std;
// for convenience
using json = nlohmann::json;

int debug = 0;
ofstream *debug_file;

int main(int argc, char **argv)
{
    string schedule_mode = "";
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
            else if (arg == "-mode")
            {
                i++;
                arg = std::string(argv[i]);
                schedule_mode = arg;
            }
            else if (arg == "-i")
            {
                i++;
                scheduler_ip = std::string(argv[i]);
            }
            else if (arg == "-p")
            {
                i++;
                scheduler_port = std::string(argv[i]);
            }
            else
            {
                cout << "Scheduler ---> main(): Error command (" << arg << ") !" << endl;
                exit(1);
            }
        }
    }
    /*
    if (schedule_mode == "")
    {
        cout << "No choose schedule mode" << endl;
        exit(1);
    }
    */
    if ((scheduler_ip == "" && scheduler_port != "") || (scheduler_ip != "" && scheduler_port == ""))
    {
        cout << "Scheduler ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if (scheduler_ip == "" && scheduler_port == "")
    {
        ifstream f("scheduler.con");

        f >> scheduler_ip;
        f >> scheduler_port;
        f.close();
        if (scheduler_ip == "" || scheduler_port == "")
        {
            cout << "scheduler.con information is error." << endl;
            exit(1);
        }
    }

    s_socket *socket = new s_socket();
    if (socket->setConnection(scheduler_ip, scheduler_port) == 0)
    {
        cout << "-1";
        return 0;
    }
    socket->closebind();

    scheduler_service *service = new scheduler_service(schedule_mode, scheduler_ip, scheduler_port);

    pid_t pid = fork();
    if (pid == -1)
    {
        cout << "-1";
        return 0;
    }
    else if (pid != 0)
    {
        cout << pid;
        return 0;
    }

    service->startservice();

    /*
    while (1)
    {
        if (socket->acceptClinet() == 1)
        {
            json request = json::parse(socket->readmessage());
            if (request["SENDER"].get<std::string>() == "server" &&
                request["RECEIVER"].get<std::string>() == "scheduler" &&
                request["REQUEST"].get<std::string>() == "do_schedule")
            {
                string server_ip = "", server_port = "";
                server_ip = request["IP"].get<std::string>();
                server_port = request["PORT"].get<std::string>();
                if (server_ip == "" || server_port == "")
                {
                    if (debug == 1)
                        *debug_file << "Scheduler ---> main_loop() : request ip port is null" << endl;
                    else if (debug == 2)
                        cout << "Scheduler ---> main_loop() : request ip port is null" << endl;
                }
                socket->closeConnection();
                scheduler_service service(schedule_mode, server_ip, server_port);
                service.do_schedule();
            }
            else
            {
                socket->closeConnection();
            }
        }
    }
    */
}