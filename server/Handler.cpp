#include "Handler.hpp"
#include <json.hpp>
#include <string>
#include "Message.hpp"
#include "c_socket.hpp"
#include "Node.hpp"
using json = nlohmann::json;
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

NewJobHandler::NewJobHandler(json job, s_socket *socket)
{
    new_job = job;
    s = socket;
}

void NewJobHandler::handle()
{
    new_job.erase("SENDER");
    new_job.erase("RECEIVER");
    new_job.erase("REQUEST");
    int jobid = Monitor::GetInstance()->addjob(new_job);
    s->sendmessage(to_string(jobid));
}

//NewJobHandler end

//RunJobHandler start

RunJobHandler::RunJobHandler(json req_run, s_socket *socket)
{
    req_runjob = req_run;
    s = socket;
}

void RunJobHandler::handle()
{
    if (req_runjob.count("JOBID") != 0)
    {
        for (int i = 0; i < (int)req_runjob["JOBID"].size(); i++)
            if (req_runjob["NPS"][i] > 0)
                Monitor::GetInstance()->setjobtoready(req_runjob["JOBID"][i].get<int>(), req_runjob["NODENAME"][i].get<std::string>(), req_runjob["NPS"][i].get<int>());
            else
                Monitor::GetInstance()->setjobtofail(req_runjob["JOBID"][i].get<int>());
    }
    else
    {
        Monitor::GetInstance()->notitfyschedualfinish();
        return;
    }
    //ERROR CONCERN
    Monitor::GetInstance()->notitfyschedualfinish();
    for (int i = 0; i < (int)req_runjob["JOBID"].size(); i++)
    {

        json jobinfo = Monitor::GetInstance()->getjobinfo(req_runjob["JOBID"][i].get<int>());
        if (jobinfo == NULL)
        {
            continue;
        }

        c_socket socket;
        Message message;
        message.msg = jobinfo;

        for (int j = 0; j < (int)(jobinfo["RUNNODE"].size()); j++)
        {
            string req_node = jobinfo["RUNNODE"][j];
            Node node = Monitor::GetInstance()->getnodeinfo(req_node);

            if (socket.setConnection(node.getnodeip(), node.getnodeport()) == 0)
            {
                if (debug > 0)
                {
                    if (debug == 1)
                        *debug_file << "Server ---> RunJobHandler handle(): setConnection() ERROR! " << endl;
                    else if (debug == 2)
                        cout << "Server ---> RunJobHandler handle(): setConnection() ERROR! " << endl;
                }
                continue;
            }
            if (socket.connect2server() == 0)
            {
                if (debug > 0)
                {
                    if (debug == 1)
                        *debug_file << "Server ---> RunJobHandler handle(): connect2server() ERROR! " << endl;
                    else if (debug == 2)
                        cout << "Server ---> RunJobHandler handle(): connect2server() ERROR! " << endl;
                }
                continue;
            }

            message.msg["MOTHERNODE"] = node.getnodename();
            break;
        }

        if (message.msg.count("MOTHERNODE") == 0)
        {
            // push_font to the joblist ??
            continue;
        }

        for (int j = 0; j < (int)(jobinfo["RUNNODE"].size()); j++)
        {
            string req_node = jobinfo["RUNNODE"][j];
            Node node = Monitor::GetInstance()->getnodeinfo(req_node);
            message.msg["RUNNODEIP"].push_back(node.getnodeip());
            message.msg["RUNNODEPORT"].push_back(node.getnodeport());
        }

        message.encode_Header("server", "mom", "runjob");
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "Server ---> RunJobHandler handle(): the message send to mom = " << endl
                            << message.encode_Message() << endl;
            else if (debug == 2)
                cout << "Server ---> RunJobHandler handle(): the message send to mom = " << endl
                     << message.encode_Message() << endl;
        }
        socket.send(message.encode_Message());
        socket.closeConnection();
        Monitor::GetInstance()->setjobtorunning(req_runjob["JOBID"][i].get<int>(), message.msg["MOTHERNODE"].get<string>());
    }
}

//RunJobHandler end

//QueueStateHandler start

QueueStateHandler::QueueStateHandler(json request, s_socket *socket)
{
    s = socket;
    req_queue_state = request;
}

void QueueStateHandler::handle()
{
    Message queuestate;
    queuestate.msg = Monitor::GetInstance()->getjobstat();
    queuestate.encode_Header("server", "scheduler", "queuestate");
    s->sendmessage(queuestate.encode_Message());
}

//QueueStateHandler end

//NodeStateHandler start

NodeStateHandler::NodeStateHandler(json request, s_socket *socket)
{
    s = socket;
    req_node_state = request;
}

void NodeStateHandler::handle()
{
    Message nodestate;
    nodestate.msg = Monitor::GetInstance()->getnodelist();
    nodestate.encode_Header("server", "scheduler", "nodestate");
    s->sendmessage(nodestate.encode_Message());
}
//NodeStateHandler end

//DoneJobHandler start
DoneJobHandler::DoneJobHandler(json request, s_socket *socket)
{
    s = socket;
    req_done_job = request;
}

void DoneJobHandler::handle()
{
    if (req_done_job.count("COMPLETEJOB") == 0)
    {
        return;
    }
    for (int i = 0; i < (int)req_done_job["COMPLETEJOB"].size(); i++)
    {
        Monitor::GetInstance()->setjobtocomplete(req_done_job["COMPLETEJOB"][i].get<int>());
    }
}
//DoneJobHandler end