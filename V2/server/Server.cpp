#include "Server.hpp"
#include <iostream>

using namespace std;

Server::Server(Monitor *m){
    cout << "attach to monitor\n";
    m->attachserver(this);
}

void Server::set_server_attr(string ip, string port){
    svr_ip = ip;
    svr_port = port;
}

void Server::set_scheduler_attr(string ip, string port){
    sch_ip = ip;
    sch_port = port;
}

void Server::notify(){
    do_schedual = true;
}

void Server::attach_success(){
    cout << "attach success\n";
}


void Server::f(int s){
	Monitor::GetInstance()->foo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}

void Server::x(int s){
	Monitor::GetInstance()->zoo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}

void Server::run(){
    pool = new ThreadPool(10);
    s_socket s;
    s.setConnection(svr_ip,svr_port);
    s.acceptClinet();
    string r = s.readmessage();
    cout << r << endl;
    s.closeConnection();
}