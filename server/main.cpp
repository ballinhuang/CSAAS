#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "Server.hpp"
#include "Monitor.hpp"
#include <mutex>
using namespace std;

mutex monitor_mtx;
Monitor *Monitor::monitor=0;
int debug = 0;
ofstream *debug_file;
int main(int argc, char **argv){
    
    if(argc > 1){
        string arg;
        arg = std::string(argv[1]);
        if(arg == "-fdebug"){
            debug = 1;
            debug_file = new ofstream();
            debug_file->open("debug.txt");
        }
        else if(arg == "-odebug"){
            debug = 2;
        }
    }

    ifstream f("server.con");
    string server_ip,server_port;
    f >> server_ip;
    f >> server_port;
    f.close();
    f.open("scheduler.con");
    string scheduler_ip,scheduler_port;
    f >> scheduler_ip;
    f >> scheduler_port;
    f.close();
    pid_t pid= fork();
	if(pid == -1) {
    	cout << "server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) {
        if(debug > 0){
            if(debug == 1)
                *debug_file << "shut down parent !" << endl;
            else if(debug == 2)
                cout << "shut down parent !" << endl;
        }
    	return 0;
    }
    //creat Server and Monitor , attach Server to Monitor.
    Monitor::GetInstance()->setnodelist();
    Server server(Monitor::GetInstance());
    server.set_server_attr(server_ip,server_port);
    server.set_scheduler_attr(scheduler_ip,scheduler_port);
    server.run();

}