#include "Server.hpp"
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

Server::Server(Monitor *m){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server Server(): Attach to monitor" << endl;
        else if(debug == 2)
            cout << "Server Server(): Attach to monitor" << endl;
    }
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
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server notify(): Receive new job!" << endl;
        else if(debug == 2)
            cout << "Server notify(): Receive new job!" << endl;
    }
    do_schedual = true;
}

void Server::attach_success(){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server attach_success():Attach success." << endl;
        else if(debug == 2)
            cout << "Server attach_success():Attach success." << endl;
    }
}

void Server::readrequest(s_socket *s, HandlerFactory *factory){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server readrequest():Start read request." << endl;
        else if(debug == 2)
            cout << "Server readrequest():Start read request." << endl;
    }
    string res;
    res = s->readmessage();
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server readrequest():Receive request = " << endl << res << endl;
        else if(debug == 2)
            cout << "Server readrequest():Receive request = " << endl << res << endl;
    }
    s->closeConnection();
    json request = json::parse(res);
    IHandler *handler =  factory->getHandler(request);
    handler->handle();
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Server readrequest():End read request." << endl;
        else if(debug == 2)
            cout << "Server readrequest():End read request." << endl;
    }
}

void Server::start_accept_thread(string ip, string port, ThreadPool* pool){
    pool = new ThreadPool(10);
    HandlerFactory *factory = new HandlerFactory();
    while(1){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server start_accept_thread(): Creat new socket." << endl;
            else if(debug == 2)
                cout << "Server start_accept_thread(): Creat new socket." << endl;
        }
        s_socket *s = new s_socket();
        if(s->setConnection(ip,port) != 1){
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "Server start_accept_thread(): setConnection() Error!" << endl;
                else if(debug == 2)
                    cout << "Server start_accept_thread(): setConnection() Error!" << endl;
            }
            continue;
        }
        if(s->acceptClinet()){
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "Server start_accept_thread(): Accept client success." << endl;
                else if(debug == 2)
                    cout << "Server start_accept_thread(): Accept client success." << endl;
            }
            s->setacceptreuse();
            s->closebind();
            pool->enqueue(&readrequest,s,factory);
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "Server start_accept_thread(): Enque readrequest() to process_pool success." << endl;
                else if(debug == 2)
                    cout << "Server start_accept_thread(): Enque readrequest() to process_pool success." << endl;
            }
        }
    }
}

void Server::run(){
    server_pool = new ThreadPool(2);
    server_pool->enqueue(start_accept_thread,svr_ip,svr_port,request_pool);
    int count =0;
    while(1){
        if(do_schedual){
            do_schedual = false;
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "conut:" << count << ": Server run(): Start send cmd to schedual." << endl;
                else if(debug == 2)
                    cout << "conut:" << count << ": Server run(): Start send cmd to schedual." << endl;
                count++;
            }
        }
    }
}