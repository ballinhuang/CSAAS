#include "DLLScheHandler.hpp"
#include "Message.hpp"

using namespace std;

DLLScheHandler::DLLScheHandler(IScheduler *scheduler, cc_socket *socket)
{
    this->scheduler = scheduler;
    this->socket = socket;
}

void DLLScheHandler::handleschedule()
{
    if(debug == 2)
        cout << "Before set information." << endl;
    // get information
    setNodeQ();
    setWaitQ();
    setRunQ();
    setTimer();

    if(debug == 2)
        cout << "Before SJF do schedule." << endl;
    // get Jobs of ready to run
    deque<Job> doJob = scheduler->schedule(waitQ, runQ, timer, nodeQ);

    if(debug == 2)
        cout << "Before SJF send result." << endl;
    // encode Jobs to Message format and send to server
    sendResult(doJob);
}

json DLLScheHandler::getInfo(string request) {
    Message reqM;
    string returnS = "";

    reqM.encode_Header("scheduler", "server", request);
    socket->send(reqM.encode_Message());
    returnS = socket->receive();
    if (returnS == "") {
        cout << "get " << request << " information error!" << endl;
        exit(1);
    }

    if(debug == 2) {
        cout << "SJF ---> request is : " << request << endl;
        cout << returnS << endl;
    }

    return json::parse(returnS);
}

void DLLScheHandler::setNodeQ() {
    Message nodeJson;
    nodeJson.msg = getInfo("nodestate");

    for(int i = 0; i < (int)nodeJson.msg["NODES"].size(); i++) {
        Node n(nodeJson.msg["NODES"][i].get<string>(), nodeJson.msg["NPS"][i].get<int>(), nodeJson.msg["ONPS"][i].get<int>());
        nodeQ.push_back(n);
    }
}

void DLLScheHandler::setWaitQ() {
    Message WaitJson;
    WaitJson.msg = getInfo("queuestate");

    for(int i = 0; i < (int)WaitJson.msg["JOBID"].size(); i++) {
        Job j(WaitJson.msg["JOBID"][i].get<long>(), WaitJson.msg["SUBMITTIME"][i].get<long>(), WaitJson.msg["RUNTIME"][i].get<long>(), WaitJson.msg["NPNEED"][i].get<int>(), wait);
        waitQ.push_back(j);
    }
}

void DLLScheHandler::setRunQ() {
    Message RunJson;
    RunJson.msg = getInfo("runqueuestate");

    for(int i = 0; i < (int)RunJson.msg["JOBID"].size(); i++)
        for(int k = 0; k < (int)RunJson.msg["RUNNODE"][i].size(); k++) {
            Job j(RunJson.msg["JOBID"][i].get<long>(), RunJson.msg["SUBMITTIME"][i].get<long>(), RunJson.msg["RUNTIME"][i].get<long>(), RunJson.msg["RUNNP"][i][k].get<int>(), RunJson.msg["RUNNODE"][i][k].get<string>(), wait);
            runQ.push_back(j);
        }
}

void DLLScheHandler::setTimer() {
    Message TimerJson;
    TimerJson.msg = getInfo("get_time");

    timer = TimerJson.msg["CURRENTTIME"].get<long>();
}

void DLLScheHandler::sendResult(deque<Job> doJob) {
    Message result;

    for(int i = 0; i < (int)doJob.size(); i++) {
        result.msg["JOBID"][i] = doJob[i].getId();
        result.msg["NODENAME"][i] = doJob[i].getNodeName();
        result.msg["NPS"][i] = doJob[i].getNp();
    }
    result.msg["TASKCOUNT"] = doJob.size();

    if(debug == 2)
        cout << "result is : " << endl << result.msg.dump() << endl;

    result.encode_Header("scheduler", "server", "runjob");
    socket->send(result.encode_Message());
}