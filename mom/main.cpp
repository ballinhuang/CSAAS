#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <s_socket.hpp>
#include <mutex>
#include "Mom.hpp"
#include "JobQueue.hpp"
using namespace std;

mutex jobqueue_mtx;
JobQueue *JobQueue::jobquque = 0;

int debug = 0;
ofstream *debug_file;
int main(int argc, char **argv)
{
    string mom_ip = "", mom_port = "";
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
                mom_ip = std::string(argv[i]);
            }
            else if (arg == "-p")
            {
                i++;
                mom_port = std::string(argv[i]);
            }
            else
            {
                cout << "MOM ---> main(): Error command (" << arg << ") !" << endl;
                exit(1);
            }
        }
    }
    if ((mom_ip == "" && mom_port != "") || (mom_ip != "" && mom_port == ""))
    {
        cout << "MOM ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    ifstream f;
    if (mom_ip == "" && mom_port == "")
    {
        f.open("mom.con");
        if (f.is_open())
        {
            f >> mom_ip;
            f >> mom_port;
        }
        else
        {
            cout << "MOM ---> main(): Error! mom.con not found." << endl;
            exit(1);
        }
        f.close();
    }

    string server_ip = "", server_port = "";

    f.open("server.con");
    if (f.is_open())
    {
        f >> server_ip;
        f >> server_port;
    }

    f.close();

    // Test socket port
    s_socket s;
    if (s.setConnection(mom_ip, mom_port) == 0)
    {
        cout << "MOM ---> main(): Error! socket bind error." << endl;
        s.closebind();
        return 0;
    }
    s.closebind();

    pid_t pid = fork();
    if (pid == -1)
    {
        cout << "mom server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if (pid != 0)
    {
        return 0;
    }
    Mom mom;
    JobQueue::GetInstance()->attachmom(&mom);
    mom.set_mom_attr(mom_ip, mom_port);
    mom.set_server_attr(server_ip, server_port);
    mom.run();
}