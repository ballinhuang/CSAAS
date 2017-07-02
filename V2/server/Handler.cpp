#include "Handler.hpp"
#include <json.hpp>
#include "Message.hpp"
#include "c_socket.hpp"
#include "Node.hpp"
using json = nlohmann::json;
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

NewJobHandler::NewJobHandler(json job, s_socket *socket){
    new_job = job;
    s = socket;
}

void NewJobHandler::handle(){
    Monitor::GetInstance()->addjob(new_job);
}

//NewJobHandler end

//RunJobHandler start

RunJobHandler::RunJobHandler(json req_run, s_socket *socket){
    req_runjob = req_run;
    s = socket;
}

void RunJobHandler::handle(){
    if(req_runjob.count("JOBID") != 0){
        int task_count = req_runjob["TASKCOUNT"].get<int>();
        for(int i = 0 ; i < task_count ; i++){
            Monitor::GetInstance()->setjobtoready(req_runjob["JOBID"][i].get<int>());
        }
    }
    Monitor::GetInstance()->notitfyschedualfinish();
    if(req_runjob.count("JOBID") != 0){
        int task_count = req_runjob["TASKCOUNT"].get<int>();
        for(int i = 0 ; i < task_count ; i++){
            string req_node = req_runjob["NODENAME"][i].get<std::string>();
            Node node = Monitor::GetInstance()->getnodeinfo(req_node);
            c_socket socket;
            if(socket.setConnection(node.getnodeip(),node.getnodeport()) == 0){
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "RunJobHandler handle(): setConnection() ERROR! " << endl;
                    else if(debug == 2)
                        cout << "RunJobHandler handle(): setConnection() ERROR! " << endl;
                }
                continue;
            }
            if(socket.connect2server() == 0){
                if(debug > 0){
                    if(debug == 1)
                        *debug_file << "RunJobHandler handle(): connect2server() ERROR! " << endl;
                    else if(debug == 2)
                        cout << "RunJobHandler handle(): connect2server() ERROR! " << endl;
                }
                continue;
            }
            Message message;
            message.msg = Monitor::GetInstance()->getjobinfo(req_runjob["JOBID"][i].get<int>());
            message.encode_Header("server","mom","runjob");
            socket.send(message.encode_Message());
            socket.closeConnection();
            Monitor::GetInstance()->setjobtorunning(req_runjob["JOBID"][i].get<int>(),req_node);
        }
    }
}

//RunJobHandler end

//QueueStateHandler start

QueueStateHandler::QueueStateHandler(json request, s_socket *socket){
    s = socket;
    req_queue_state = request;
}

void QueueStateHandler::handle(){
    Message queuestate;
    queuestate.msg = Monitor::GetInstance()->getjobstat();
    queuestate.encode_Header("server","scheduler","queuestate");
    s->sendmessage(queuestate.encode_Message());
}

//QueueStateHandler end

//NodeStateHandler start

NodeStateHandler::NodeStateHandler(json request, s_socket *socket){
    s = socket;
    req_node_state = request;
}

void NodeStateHandler::handle(){
    Message nodestate;
    nodestate.msg = Monitor::GetInstance()->getnodelist();
    nodestate.encode_Header("server","scheduler","nodestate");
    s->sendmessage(nodestate.encode_Message());
}
//NodeStateHandler end