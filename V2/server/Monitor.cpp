#include "Monitor.hpp"
#include "Node.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;
extern mutex monitor_mtx;

Monitor *Monitor::GetInstance(){
    if (monitor == NULL)
    {
        monitor_mtx.lock();
        if (monitor == NULL)
        {
            monitor = new Monitor();
        }
        monitor_mtx.unlock();
    }
    return monitor;
}

void Monitor::attachserver(Observer *obs) {
    observer = obs;
    obs->attach_success();
}

void Monitor::notitfynewjob(){
    observer->notify(0);
}

void Monitor::notitfyschedualfinish(){
    observer->notify(1);
}

void Monitor::addjob(json newjob){
    jobtex.lock();
    newjob["JOBID"] = jobcount;
    newjob["JOBSTAT"] = "WAITE";
    jobcount++;
    joblist.push_back(newjob);
    notitfynewjob();
    jobtex.unlock();
}

json Monitor::getjobstat(){
    json result;
    jobtex.lock();
    int i = 0;
    for(vector<json>::iterator it = joblist.begin() ; it != joblist.end() ; it++){
        result["JOBID"][i] = (*it)["JOBID"];
        i++;
    }
    jobtex.unlock();
    return result;
}

void Monitor::setnodelist(){
    ifstream nodes_fd;
    nodes_fd.open("node.con");
    string ip,port,name;
    //error concern
    while( nodes_fd >> ip >> port >> name){
        Node node;
        node.setnodeip(ip);
        node.setnodeport(port);
        node.setnodename(name);
        nodelist[name] = node;
    }
}

json Monitor::getnodelist(){
    json result;
    int i = 0;
    for(map<string,Node>::iterator it = nodelist.begin() ; it != nodelist.end() ; it++){
        result["NODENAME"][i] = it->second.getnodename();
        result["NODEIP"][i] = it->second.getnodeip();
        result["NODEPORT"][i] = it->second.getnodeport();
        i++;
    }
    return result;
}