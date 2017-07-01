#include <json.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "s_socket.hpp"
#include "scheduler_service.hpp"

using namespace std;

int debug = 0;
ofstream *debug_file;


int main(int argc, char **argv){
    string schedule_mode = "";
    if(argc > 1){
        string arg;
        for(int i = 1 ; i <= argc ; i++){
            arg = std::string(argv[i]);
            if(arg == "-fdebug"){
                debug = 1;
                debug_file = new ofstream();
                debug_file->open("debug.txt");
            }
            else if(arg == "-odebug"){
                debug = 2;
            }
            else if(arg == "-mode"){
                i++;
                arg = std::string(argv[i]);
                schedule_mode = arg;
            }
        }
    }

    if(schedule_mode == ""){
        cout << "No choose schedule mode" << endl;
        exit(1);
    }

    ifstream f("scheduler.con");
    string scheduler_ip = "",scheduler_port = "";
    f >> scheduler_ip;
    f >> scheduler_port;
    f.close();

    if(scheduler_ip == "" || scheduler_port == ""){
        cout << "scheduler.con information is error." << endl;
        exit(1);
    }

    s_socket *socket = new s_socket();
    if(*socket->setConnection(scheduler_ip,scheduler_port) == 0){
        cout << "Socket creat error !" << endl;
        exit(1);
    }

    pid_t pid= fork();
	if(pid == -1) {
    	cout << "Scheduler fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) {
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler shut down parent !" << endl;
            else if(debug == 2)
                cout << "Scheduler shut down parent !" << endl;
        }
    	return 0;
    }

    while(1){
        if(*socket->acceptClinet() == 1){
            json request = json::parse(*socket->readmessage());
            if(request["SENDER"].get<std::string>()   == "server"       &&
               request["RECEIVER"].get<std::string>() == "scheduler"    && 
               request["REQUEST"].get<std::string>()  == "do_schedual"){
                string server_ip,server_port;
                server_ip = *socket->getClientIP()
                server_port = *socket->getClientPort();
                *socket->closeConnection();
                scheduler_service service(schedule_mode,server_ip,server_port);
                service.do_schedual();
            }
            else{
                *socket->closeConnection();
            }
        }
    }
}