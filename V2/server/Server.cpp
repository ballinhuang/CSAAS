#include "Server.hpp"
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;
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
    cout << "recuve new job!\n";
    do_schedual = true;
}

void Server::attach_success(){
    cout << "attach success\n";
}

void Server::readrequest(s_socket *s){
    string res;
    res = s->readmessage();
    s->closeConnection();
    json job = json::parse(res);
    Monitor::GetInstance()->addjob(job);
}

void Server::start_accept_thread(string ip, string port, ThreadPool* pool){
    pool = new ThreadPool(10);
    while(1){
        s_socket s;
        s.setConnection(ip,port);
        if(s.acceptClinet()){
            s.setacceptreuse();
            pool->enqueue(&readrequest,&s);
        }
        s.closebind();
    }
}

void Server::run(){
    server_pool = new ThreadPool(2);
    server_pool->enqueue(start_accept_thread,svr_ip,svr_port,request_pool);
    while(1){

    }
}