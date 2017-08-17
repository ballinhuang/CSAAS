#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
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

    if (argc > 1)
    {
        string arg;
        arg = std::string(argv[1]);
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
        else
        {
            cout << "MOM ---> main(): Error command (" << arg << ") !" << endl;
            exit(1);
        }
    }

    string server_ip, server_port;
    ifstream f("server.con");
    if (f.is_open())
    {
        f >> server_ip;
        f >> server_port;
    }
    else
    {
        cout << "MOM ---> main(): Error! server.con not found." << endl;
        exit(1);
    }
    f.close();
    string mom_ip, mom_port;
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

    pid_t pid = fork();
    if (pid == -1)
    {
        cout << "mom server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if (pid != 0)
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "mom shut down parent !" << endl;
            else if (debug == 2)
                cout << "mom shut down parent !" << endl;
        }
        return 0;
    }
    Mom mom;
    JobQueue::GetInstance()->attachmom(&mom);
    mom.set_server_attr(server_ip, server_port);
    mom.set_mom_attr(mom_ip, mom_port);
    mom.run();
}