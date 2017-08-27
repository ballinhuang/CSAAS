#include "FIFOScheHandler.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include "Message.hpp"

using namespace std;
// for convenience
using json = nlohmann::json;

extern int debug;
extern ofstream *debug_file;

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
    req_runjob.initMessage();
    req_runjob.msg["TASKCOUNT"] = 0;
    if(queuestate.count("JOBID") > 0){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "*****Scheduler ---> FIFOScheHandler handleschedule(): Start Scheduling.*****" << endl;
            else if(debug == 2)
                cout << "*****Scheduler ---> FIFOScheHandler handleschedule(): Start Scheduling.*****" << endl;
        }

        for(int queue_index = 0 ; queue_index < (int)queuestate["JOBID"].size() ; queue_index++){
            if(queuestate["NODENEED"][queue_index].get<int>() == 0) {
                if(handleUnsignNode(req_runjob ,queuestate, nodestate, queue_index))
                    break;
            }
            else if(queuestate["NODENEED"][queue_index].get<int>() == 1) {
                if(handleSingleNode(req_runjob ,queuestate, nodestate, queue_index))
                    break;
            }
            else if(queuestate["NODENEED"][queue_index].get<int>() > 1) {
                if(handleMultiNode(req_runjob ,queuestate, nodestate, queue_index))
                    break;
            }
            else {
                if(debug == 1)
                    *debug_file << "Scheduler ---> JOB" << queuestate["JOBID"] << "'s node is less then 0." << endl;
                else if(debug == 2)
                    cout << "Scheduler ---> JOB" << queuestate["JOBID"] << "'s node is less then 0." << endl;

                break;
            }

            if(debug == 1)
                *debug_file << "Scheduler ---> Req_runjob: " << req_runjob.msg.dump() << endl;
            else if(debug == 2)
                cout << "Scheduler ---> Req_runjob: " << req_runjob.msg.dump() << endl;
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

bool FIFOScheHandler::handleUnsignNode(Message &req, json &queue, json &node, int queue_index) {
    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleUnsignNode(): Start Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleUnsignNode(): Start Scheduling." << endl;

    int node_index = 0, NPneed = 0;
    map<string, int> nodeByNP;
    map<string, int>::iterator mi;

    for(NPneed = queue["NPNEED"][queue_index].get<int>(), node_index = 0; NPneed > 0 && node_index < (int)node["NODES"].size(); node_index++)
        NPneed -= node["NPS"][node_index].get<int>();
    if(NPneed > 0) {
        req.msg["JOBID"][queue_index] = queue["JOBID"][queue_index];
        req.msg["NODENAME"][queue_index] = "FAIL";
        req.msg["NPS"][queue_index] = -1;
        req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
        if(debug == 1)
            *debug_file << "Scheduler ---> There are not enough NPs(all) for JOB" << queue_index << " to execute." << endl;
        else if(debug == 2)
            cout << "Scheduler ---> There are not enough NPs(all) for JOB" << queue_index << " to execute." << endl;
        return false;
    }

    for(NPneed = queue["NPNEED"][queue_index].get<int>(), node_index = 0; NPneed > 0 && node_index < (int)node["NODES"].size(); node_index++) {
        for(mi = nodeByNP.begin(); mi != nodeByNP.end(); mi++)
            if(mi->first == node["NODES"][node_index].get<string>())
                break;
        if(mi != nodeByNP.end() || node["NPS"][node_index].get<int>() == 0)
            continue;

        if(NPneed < node["NPS"][node_index].get<int>()) {
            nodeByNP[node["NODES"][node_index].get<string>()] = NPneed;
            NPneed = 0;
        }
        else {
            nodeByNP[node["NODES"][node_index].get<string>()] = node["NPS"][node_index].get<int>();
            NPneed -= node["NPS"][node_index].get<int>();
        }
    }

    if(NPneed > 0) {
        if(debug == 1)
            *debug_file << "Scheduler ---> There are not enough NPs(now) for JOB" << queue_index << " to execute." << endl;
        else if(debug == 2)
            cout << "Scheduler ---> There are not enough NPs(now) for JOB" << queue_index << " to execute." << endl;
        return true;
    }

    for(node_index = 0; node_index < (int)node["NODES"].size(); node_index++) {
        mi = nodeByNP.find(node["NODES"][node_index].get<string>());
        if(mi != nodeByNP.end())
            node["NPS"][node_index] = node["NPS"][node_index].get<int>() - mi->second;
    }

    for(mi = nodeByNP.begin(); mi != nodeByNP.end(); mi++) {
        req.msg["JOBID"].push_back(queue["JOBID"][queue_index]);
        req.msg["NODENAME"].push_back(mi->first);
        req.msg["NPS"].push_back(mi->second);
    }
    req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
    
    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleUnsignNode(): End Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleUnsignNode(): End Scheduling." << endl;

    return false;
}

bool FIFOScheHandler::handleSingleNode(Message &req, json &queue, json &node, int queue_index) {
    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleSingleNode(): Start Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleSingleNode(): Start Scheduling." << endl;

    int node_index = 0;
    bool can_not_do = true;

    for(node_index = 0; node_index < (int)node["NODES"].size(); node_index++){
        if(queue["NPNEED"][queue_index].get<int>() <= node["ONPS"][node_index].get<int>())
            can_not_do = false;

        if(queue["NPNEED"][queue_index].get<int>() <= node["NPS"][node_index].get<int>()){
            req.msg["JOBID"].push_back(queue["JOBID"][queue_index]);
            req.msg["NODENAME"].push_back(node["NODES"][node_index]);
            req.msg["NPS"].push_back(queue["NPNEED"][queue_index].get<int>());
            node["NPS"][node_index] = node["NPS"][node_index].get<int>() - queue["NPNEED"][queue_index].get<int>();
            if(debug == 1) {
                *debug_file << "Scheduler ---> Assin " << req.msg["JOBID"][queue_index] << " to " << req.msg["NODENAME"][queue_index] << endl;
                *debug_file << "Now nodes' state is: " << node.dump() << endl;
            }
            else if(debug == 2) {
                cout << "Scheduler ---> Assin " << req.msg["JOBID"][queue_index] << " to " << req.msg["NODENAME"][queue_index] << endl;
                cout << "Now nodes' state is: " << node.dump() << endl;
            }
            req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
            break;
        }
    }

    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleSingleNode(): End Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleSingleNode(): End Scheduling." << endl;

    if(can_not_do) {
        req.msg["JOBID"][queue_index] = queue["JOBID"][queue_index];
        req.msg["NODENAME"][queue_index] = "FAIL";
        req.msg["NPS"][queue_index] = -1;
        req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
        if(debug == 1)
            *debug_file << "Scheduler ---> JOB" << queue_index << " can't execute on any node." << endl;
        else if(debug == 2)
            cout << "Scheduler ---> JOB" << queue_index << " can't execute on any node." << endl;
        return false;
    }
    if(node_index == (int)node["NODES"].size())
        return true;
    else
        return false;
}

bool FIFOScheHandler::handleMultiNode(Message &req, json &queue, json &node, int queue_index) {
    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleMultiNode(): Start Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleMultiNode(): Start Scheduling." << endl;

    int node_index = 0, NPneed_index = 0;
    int NPneed[100] = {0};
    bool can_not_do = true;
    map<string, int> nodeByNP;
    map<string, int>::iterator mi;

    if((int)queue["NPNEED"][queue_index].size() == 1)
        for(int i = 0; i < queue["NODENEED"][queue_index].get<int>(); i++)
            NPneed[i] = queue["NPNEED"][queue_index].get<int>();
    else
        for(int i = 0; i < (int)queue["NPNEED"][queue_index].size(); i++)
            NPneed[i] = queue["NPNEED"][queue_index][i].get<int>();

    for(NPneed_index = 0; NPneed_index < (int)queue["NODENEED"][queue_index].get<int>(); NPneed_index++) {
        can_not_do = true;
        for(node_index = 0; node_index < (int)node["NODES"].size(); node_index++) {
            for(mi = nodeByNP.begin(); mi != nodeByNP.end(); mi++)
                if(mi->first == node["NODES"][node_index].get<string>())
                    break;
            if(mi != nodeByNP.end())
                continue;

            if(NPneed[NPneed_index] <= node["ONPS"][node_index].get<int>())
                can_not_do = false;

            if(NPneed[NPneed_index] <= node["NPS"][node_index].get<int>()) {
                nodeByNP[node["NODES"][node_index].get<string>()] = NPneed[NPneed_index];
                break;
            }
        }

        if(can_not_do) {
            req.msg["JOBID"][queue_index] = queue["JOBID"][queue_index];
            req.msg["NODENAME"][queue_index] = "FAIL";
            req.msg["NPS"][queue_index] = -1;
            req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
            if(debug == 1)
                *debug_file << "Scheduler ---> JOB" << queue_index << " can't execute on any node." << endl;
            else if(debug == 2)
                cout << "Scheduler ---> JOB" << queue_index << " can't execute on any node." << endl;
            return false;
        }
        if(node_index == (int)node["NODES"].size())
            return true;
    }

    for(node_index = 0; node_index < (int)node["NODES"].size(); node_index++) {
        mi = nodeByNP.find(node["NODES"][node_index].get<string>());
        if(mi != nodeByNP.end())
            node["NPS"][node_index] = node["NPS"][node_index].get<int>() - mi->second;
    }

    for(mi = nodeByNP.begin(); mi != nodeByNP.end(); mi++) {
        req.msg["JOBID"].push_back(queue["JOBID"][queue_index]);
        req.msg["NODENAME"].push_back(mi->first);
        req.msg["NPS"].push_back(mi->second);
    }
    req.msg["TASKCOUNT"] = req.msg["TASKCOUNT"].get<int>() + 1;
    
    if(debug == 1)
        *debug_file << "Scheduler ---> FIFOScheHandler handleMultiNode(): End Scheduling." << endl;
    else if(debug == 2)
        cout << "Scheduler ---> FIFOScheHandler handleMultiNode(): End Scheduling." << endl;

    return false;
}