#include "Server.hpp"
#include <iostream>
#include <json.hpp>
#include "c_socket.hpp"
#include "Message.hpp"
#include <unistd.h>

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

void Server::notify(int msg){
    // msg == 0 notitfy newjob, msg == 1 notitfy schedual finish
    //sleep(1);
    if(msg == 0){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server notify(0): Receive new job!" << endl;
            else if(debug == 2)
                cout << "Server notify(0): Receive new job!" << endl;
        }
        do_schedual_tex.lock();
        do_schedual = true;
        do_schedual_tex.unlock();
    }
    else if(msg == 1){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server notify(1): Schedualer finish schedual!" << endl;
            else if(debug == 2)
                cout << "Server notify(1): Schedualer finish schedual!" << endl;
        }
        schedual_busy_tex.lock();
        schedual_busy = false;
        schedual_busy_tex.unlock();
    }
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
    
    while( 1 ){
        string res = "";
        res = s->readmessage();
        if(res == ""){
            break;
        }
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server readrequest():Receive request = " << endl << res << endl;
            else if(debug == 2)
                cout << "Server readrequest():Receive request = " << endl << res << endl;
        }
        json request = json::parse(res);
        IHandler *handler =  factory->getHandler(request,s);
        if(handler == NULL){
            break;
        }
        handler->handle();
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server readrequest():End read request." << endl;
            else if(debug == 2)
                cout << "Server readrequest():End read request." << endl;
        }
    }
    s->closeConnection();
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

void Server::check_schedule(){
    now_time = time(NULL);
    if(next_schedule - now_time <= 0){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server check_schedulen(): Trigger to do scheduale." << endl;
            else if(debug == 2)
                cout << "Server check_schedulen(): Trigger to do scheduale." << endl;
        }
        do_schedual_tex.lock();
        do_schedual = true;
        do_schedual_tex.unlock();
    }
}

void Server::run(){
    server_pool = new ThreadPool(2);
    server_pool->enqueue(start_accept_thread,svr_ip,svr_port,request_pool);
    sched_iteration = 60; //1 min
    now_time = time(NULL);
    next_schedule = now_time + sched_iteration;
    int count =0;
    while(1){
        check_schedule();
        if(do_schedual){
            next_schedule = now_time + sched_iteration;
            if(!schedual_busy){
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "conut:" << count << ": Server run(): Start send cmd to schedual." << endl;
                    else if(debug == 2)
                        cout << "conut:" << count << ": Server run(): Start send cmd to schedual." << endl;
                    count++;
                }
                if(contact_scheduler() == 1){
                    do_schedual_tex.lock();
                    do_schedual = false;
                    do_schedual_tex.unlock();
                    schedual_busy_tex.lock();
                    schedual_busy = true;
                    schedual_busy_tex.unlock();
                    if(debug > 0){
                        if(debug == 1)
                            *debug_file << "Server run(): Contact_scheduler() sccess." << endl;
                        else if(debug == 2)
                            cout << "Server run(): Contact_scheduler() sccess." << endl;
                    }
                }
                else{
                    if(debug > 0){
                        if(debug == 1)
                            *debug_file << "Server run(): Contact_scheduler() fail." << endl;
                        else if(debug == 2)
                            cout << "Server run(): Contact_scheduler() fail." << endl;
                    }
                    do_schedual_tex.lock();
                    do_schedual = false;
                    do_schedual_tex.unlock();
                }
            }
            else{
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "Server run(): Scheduler is busy." << endl;
                    else if(debug == 2)
                        cout << "Server run(): Scheduler is busy." << endl;
                }
                do_schedual_tex.lock();
                do_schedual = false;
                do_schedual_tex.unlock();
            }
        }
    }
}

int Server::contact_scheduler(){
    c_socket s;
    Message do_schedule_msg;
    if(s.setConnection(sch_ip,sch_port) != 1){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server contact_scheduler(): Server client socket creat error !" << endl;
            else if(debug == 2)
                cout << "Server contact_scheduler(): Server client socket creat error !" << endl;
        }
        return 0;
    }
    if(s.connect2server() != 1){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Server contact_scheduler(): Server client socket connect error !" << endl;
            else if(debug == 2)
                cout << "Server contact_scheduler(): Server client socket connect error !" << endl;
        }
        return 0;
    }
    do_schedule_msg.initMessage();
    do_schedule_msg.msg["IP"] = svr_ip;
    do_schedule_msg.msg["PORT"] = svr_port;
    do_schedule_msg.encode_Header("server","scheduler","do_schedule");
    s.send(do_schedule_msg.encode_Message());
    s.closeConnection();
    return 1;
}