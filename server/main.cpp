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
        arg = string(argv[1]);
        if(arg == "-fdebug"){
            debug = 1;
            debug_file = new ofstream();
            debug_file->open("debug.txt");
        }
        else if(arg == "-odebug"){
            debug = 2;
        }
        else{
            cout << "Server ---> main(): Error command ("<< arg << ") !" << endl;
            exit(1);
        }
    }

    ifstream f("server.con");
    string server_ip,server_port;
    if(f.is_open()){
        f >> server_ip;
        f >> server_port;
    }
    else{
        cout << "Server ---> main(): Error! server.con not found." << endl;
        exit(1);
    }
    f.close();
    f.open("scheduler.con");
    string scheduler_ip,scheduler_port;
    if(f.is_open()){
        f >> scheduler_ip;
        f >> scheduler_port;
    }
    else{
        cout << "Server ---> main(): Error! scheduler.con not found." << endl;
        exit(1);
    }
    f.close();

    //creat Server and Monitor , attach Server to Monitor.
    Monitor::GetInstance()->setnodelist();
    Server server(Monitor::GetInstance());

    pid_t pid= fork();
	if(pid < 0) {
    	cout << "Server ---> main(): Fork Error !" << endl;
        exit(1);
    }
    else if(pid != 0) {
    	return 0;
    }
    
    server.set_server_attr(server_ip,server_port);
    server.set_scheduler_attr(scheduler_ip,scheduler_port);
    server.run();

}