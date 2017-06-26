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

int main(int argc, char **argv){
    
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
    	cout << "shut down parent !" << endl;
    	return 0;
    }
    //creat Server and Monitor , attach Server to Monitor.
    Server server(Monitor::GetInstance());
    server.set_server_attr(server_ip,server_port);
    server.set_scheduler_attr(scheduler_ip,scheduler_port);
    server.run();

}