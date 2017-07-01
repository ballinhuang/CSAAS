#include "Handler.hpp"
#include <json.hpp>
#include "Message.hpp"
using json = nlohmann::json;
using namespace std;

extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

NewJobHandler::NewJobHandler(json job, s_socket *socket){
    new_job = job;
    s = socket;
}

void NewJobHandler::handle(){
    Monitor::GetInstance()->addjob(new_job);
    s->closeConnection();
}

//NewJobHandler end

//RunJobHandler start

RunJobHandler::RunJobHandler(json req_run, s_socket *socket){
    req_runjob = req_run;
    s = socket;
}

void RunJobHandler::handle(){
    Monitor::GetInstance()->notitfyschedualfinish();
    s->closeConnection();
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
    s->closeConnection();
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
    s->closeConnection();
}
//NodeStateHandler end