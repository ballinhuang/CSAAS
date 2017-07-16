#include "Mom.hpp"
#include <iostream>
#include <json.hpp>
#include "c_socket.hpp"
#include "Message.hpp"
#include "JobQueue.hpp"
#include <unistd.h>

using json = nlohmann::json;
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

void Mom::set_server_attr(string ip, string port){
    svr_ip = ip;
    svr_port = port;
}

void Mom::set_mom_attr(string ip, string port){
    mom_ip = ip;
    mom_port = port;
}

void Mom::notify(int msg){
    // msg == 0 notitfy
    if(msg == 0){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "MOM ---> MOM notify(0): Job finish!" << endl;
            else if(debug == 2)
                cout << "MOM ---> MOM notify(0): Job finish!" << endl;
        }
        job_down_tex.lock();
        job_down = true;
        job_down_tex.unlock();
    }
}

void Mom::attach_success(){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "MOM ---> MOM attach_success():Attach success." << endl;
        else if(debug == 2)
            cout << "MOM ---> MOM attach_success():Attach success." << endl;
    }
}

void Mom::readrequest(s_socket *s, MomHandlerFactory *factory){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "MOM ---> Mom readrequest():Start read request." << endl;
        else if(debug == 2)
            cout << "MOM ---> Mom readrequest():Start read request." << endl;
    }
    
    while( 1 ){
        string res = "";
        res = s->readmessage();
        if(res == ""){
            break;
        }
        if(debug > 0){
            if(debug == 1)
                *debug_file << "MOM ---> Mom readrequest():Receive request = " << endl << res << endl;
            else if(debug == 2)
                cout << "MOM ---> Mom readrequest():Receive request = " << endl << res << endl;
        }
        json request = json::parse(res);
        IHandler *handler =  factory->getHandler(request,s);
        if(handler == NULL){
            break;
        }
        handler->handle();
        if(debug > 0){
            if(debug == 1)
                *debug_file << "MOM ---> Mom readrequest():End read request." << endl;
            else if(debug == 2)
                cout << "MOM ---> Mom readrequest():End read request." << endl;
        }
    }
    s->closeConnection();
}

void Mom::start_accept_thread(string ip, string port, ThreadPool* pool){
    pool = new ThreadPool(10);
    MomHandlerFactory *factory = new MomHandlerFactory();
    while(1){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "MOM ---> Mom start_accept_thread(): Creat new socket." << endl;
            else if(debug == 2)
                cout << "MOM ---> Mom start_accept_thread(): Creat new socket." << endl;
        }
        s_socket *s = new s_socket();
        if(s->setConnection(ip,port) != 1){
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "MOM ---> Mom start_accept_thread(): setConnection() Error!" << endl;
                else if(debug == 2)
                    cout << "MOM ---> Mom start_accept_thread(): setConnection() Error!" << endl;
            }
            continue;
        }
        if(s->acceptClinet()){
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "MOM ---> Mom start_accept_thread(): Accept client success." << endl;
                else if(debug == 2)
                    cout << "MOM ---> Mom start_accept_thread(): Accept client success." << endl;
            }
            s->setacceptreuse();
            s->closebind();
            pool->enqueue(&readrequest,s,factory);
            if(debug > 0){
                if(debug == 1)
                    *debug_file << "MOM ---> Mom start_accept_thread(): Enque readrequest() to process_pool success." << endl;
                else if(debug == 2)
                    cout << "MOM ---> Mom start_accept_thread(): Enque readrequest() to process_pool success." << endl;
            }
        }
    }
}


void Mom::run(){
    mom_pool = new ThreadPool(1);
    mom_pool->enqueue(start_accept_thread,mom_ip,mom_port,request_pool);
    while(1){
        job_down_tex.lock();
        if(job_down){
            job_down = false;
            job_down_tex.unlock();
            //contact to server
            Message donejob_msg;
            c_socket socket;
            if(socket.setConnection(svr_ip,svr_port) == 0){
                continue;
            }
            if(socket.connect2server() == 0){
                continue;
            }
            donejob_msg.msg = JobQueue::GetInstance()->getdonejob();
            donejob_msg.encode_Header("mom","server","donejob");
            socket.send(donejob_msg.encode_Message());
            socket.closeConnection();
        }
        else{
            job_down_tex.unlock();
        }
    }
}