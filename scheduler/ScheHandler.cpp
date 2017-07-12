#include "ScheHandler.hpp"
#include <fstream>
#include <iostream>
#include "Message.hpp"

using namespace std;
// for convenience
using json = nlohmann::json;

extern int debug;
extern ofstream *debug_file;
//FIFOScheHandler start

FIFOScheHandler::FIFOScheHandler(cc_socket *sock){
    socket = sock;
}

void FIFOScheHandler::handleschedule(){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): Start handle schedule." << endl;
        else if(debug == 2)
            cout << "Scheduler ---> FIFOScheHandler handleschedule(): Start handle schedule." << endl;
    }
    string str_nodestate = "", str_queuestate = "";
    json nodestate,queuestate;
    Message req_nodestate,req_queuestate;

    req_nodestate.encode_Header("scheduler","server","nodestate");
    socket->send(req_nodestate.encode_Message());
    str_nodestate = socket->receive();
    if(str_nodestate != ""){
        nodestate = json::parse(str_nodestate);
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): Require nodestate = " << endl << nodestate.dump() << endl;
            else if(debug == 2)
                cout << "Scheduler ---> FIFOScheHandler handleschedule(): Require nodestate = " << endl << nodestate.dump() << endl;
        }
    }
    
    req_queuestate.encode_Header("scheduler","server","queuestate");
    socket->send(req_queuestate.encode_Message());
    str_queuestate = socket->receive();
    if(str_queuestate != ""){
        queuestate = json::parse(str_queuestate);
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): Require queuestate = " << endl << queuestate.dump() << endl;
            else if(debug == 2)
                cout << "Scheduler ---> FIFOScheHandler handleschedule(): Require queuestate = " << endl << queuestate.dump() << endl;
        }
    }

    if(str_nodestate == "" || str_queuestate == ""){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): ERROR Information not enough!" << endl;
            else if(debug == 2)
                cout << "Scheduler ---> FIFOScheHandler handleschedule(): ERROR Information not enough!" << endl;
        }
        return;
    }

    Message req_runjob;
    if(queuestate.count("JOBID") > 0){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): Start Scheduling." << endl;
            else if(debug == 2)
                cout << "Scheduler ---> FIFOScheHandler handleschedule(): Start Scheduling." << endl;
        }

        for(int i = 0 ; i < (int)queuestate["JOBID"].size() ; i++){
            for(int j = 0; j < (int)nodestate["NODES"].size() ; j++){
                req_runjob.msg["JOBID"][i] = queuestate["JOBID"][i];
                req_runjob.msg["NODENAME"][i] = nodestate["NODES"][j][1];
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "Scheduler ---> Assin " << req_runjob.msg["JOBID"][i] << " to " << req_runjob.msg["NODENAME"][i] << endl;
                    else if(debug == 2)
                        cout << "Scheduler ---> Assin " << req_runjob.msg["JOBID"][i] << " to " << req_runjob.msg["NODENAME"][i] << endl;
                }
                i++;
                req_runjob.msg["TASKCOUNT"] = i;
                if(i >= (int)queuestate["JOBID"].size())
                    break;
            }
            if(i >= (int)queuestate["JOBID"].size())
                break;
            i--;
        }
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): End Scheduling." << endl;
            else if(debug == 2)
                cout << "Scheduler ---> FIFOScheHandler handleschedule(): End Scheduling. result =" << endl << req_runjob.encode_Message() << endl;
        }
    }
    
    req_runjob.encode_Header("scheduler","server","runjob");
    
    socket->send(req_runjob.encode_Message());
    socket->closeConnection();

}

//FIFOScheHandler end