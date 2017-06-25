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
    cout << "Monitor get a job!\n";
}

void Server::attach_success(){
    cout << "attach success\n";
}

//test
void Server::f(int s){
	Monitor::GetInstance()->foo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}
//test
void Server::x(int s){
	Monitor::GetInstance()->zoo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}

void Server::run(){
    ThreadPool pool(10);
    for(int i = 1; i <= 20; ++i) {
		int arg = i;
		if(i % 5 != 0)
		pool.enqueue(&Server::f,arg);
		else
		pool.enqueue(&Server::x,arg);
	}
}