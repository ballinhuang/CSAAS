#include "Monitor.hpp"
#include "Node.hpp"
#include <ThreadPool.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;
extern mutex monitor_mtx;

extern int debug;
extern ofstream *debug_file;

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
    joblist[jobcount] = newjob;
    jobcount++;
    notitfynewjob();
    jobtex.unlock();
}

void Monitor::setjobtoready(int jobid, string node){
    map<int , json>::iterator iter;
    jobtex.lock();
    iter = joblist.find(jobid);
    if(iter != joblist.end()){
        readytex.lock();
        (iter->second)["JOBSTAT"] = "READY";
        readylist[jobid] = iter->second;
        (readylist[jobid])["RUNNODE"].push_back(node);
        readytex.unlock();
        joblist.erase(iter);
        jobtex.unlock();
    }
    else{
        jobtex.unlock();
        readytex.lock();
        (runninglist[jobid])["RUNNODE"].push_back(node);
        readytex.unlock();
    }
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Monitor setjobtoready(): Move job to ready queue  jobid = " << jobid << " content = " << readylist[jobid].dump() << endl;
        else if(debug == 2)
            cout << "Monitor setjobtoready(): Move job to ready queue  jobid = " << jobid << " content = " << readylist[jobid].dump() << endl;
    }
}

void Monitor::setjobtorunning(int jobid,string mothor){
    map<int , json>::iterator iter;
    readytex.lock();
    iter = readylist.find(jobid);
    if(iter == readylist.end()){
        readytex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "RUNNING";
    (iter->second)["MOTHERNODE"] = mothor;
    runningtex.lock();
    runninglist[jobid] = iter->second;
    runningtex.unlock();
    readylist.erase(iter);
    readytex.unlock();
    
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Monitor setjobtorunning(): Move job to running queue  jobid = " << jobid << " content = " << runninglist[jobid].dump() << endl;
        else if(debug == 2)
            cout << "Monitor setjobtorunning(): Move job to running queue  jobid = " << jobid << " content = " << runninglist[jobid].dump() << endl;
    }
}

json Monitor::getjobstat(){
    json result;
    jobtex.lock();
    int i = 0;
    for(map<int,json>::iterator it = joblist.begin() ; it != joblist.end() ; it++){
        result["JOBID"][i] = (it->second)["JOBID"];
        i++;
    }
    jobtex.unlock();
    result["JOBCOUNT"] = i;
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
    result["NODECOUNT"] = i;
    return result;
}

Node Monitor::getnodeinfo(string nodename){
    return nodelist[nodename];
}

json Monitor::getjobinfo(int jobid){
    map<int , json>::iterator iter;
    json result;
    readytex.lock();
    iter = readylist.find(jobid);
    if(iter == readylist.end()){
        readytex.unlock();
        return NULL;
    }
    result = iter->second;
    readytex.unlock();
    return result;
}