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

Monitor *Monitor::GetInstance()
{
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

void Monitor::attachserver(Observer *obs)
{
    observer = obs;
    obs->attach_success();
}

void Monitor::notitfynewjob()
{
    observer->notify(0);
}

void Monitor::notitfyschedualfinish()
{
    observer->notify(1);
}

int Monitor::addjob(json newjob)
{
    int jobid;
    jobtex.lock();
    jobid = jobcount;
    newjob["JOBID"] = jobcount;
    newjob["JOBSTAT"] = "WAIT";
    joblist[jobcount] = newjob;
    jobcount++;
    jobtex.unlock();
    notitfynewjob();
    return jobid;
}

void Monitor::setjobtoready(int jobid, string node, int np)
{
    map<int, json>::iterator iter;
    map<std::string, Node>::iterator it;
    jobtex.lock();
    iter = joblist.find(jobid);
    if (iter != joblist.end())
    {
        (iter->second)["JOBSTAT"] = "READY";
        readylist[jobid] = iter->second;
        (readylist[jobid])["RUNNODE"].push_back(node);
        (readylist[jobid])["RUNNP"].push_back(np);
        joblist.erase(iter);
    }
    else
    {
        (readylist[jobid])["RUNNODE"].push_back(node);
        (readylist[jobid])["RUNNP"].push_back(np);
    }

    it = nodelist.find(node);
    cout << "***now node CPU = " << (it->second).getnodeCPUcore() << ", need CPU = " << np << "***" << endl;
    (it->second).setCPUcore((it->second).getnodeCPUcore() - np);
    jobtex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtoready(): Move job to ready queue  jobid = " << jobid << " content = " << readylist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtoready(): Move job to ready queue  jobid = " << jobid << " content = " << readylist[jobid].dump() << endl;
    }
}

void Monitor::setjobtorunning(int jobid, string mothor)
{
    map<int, json>::iterator iter;
    readytex.lock();
    iter = readylist.find(jobid);
    if (iter == readylist.end())
    {
        readytex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "RUNNING";
    (iter->second)["MOTHERNODE"] = mothor;
    runninglist[jobid] = iter->second;
    readylist.erase(iter);
    readytex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtorunning(): Move job to running queue  jobid = " << jobid << " content = " << runninglist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtorunning(): Move job to running queue  jobid = " << jobid << " content = " << runninglist[jobid].dump() << endl;
    }
}

void Monitor::setjobtocomplete(int jobid)
{
    map<int, json>::iterator iter;
    map<std::string, Node>::iterator it;
    runningtex.lock();
    iter = runninglist.find(jobid);
    if (iter == runninglist.end())
    {
        runningtex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "COMPLETE";
    completelist[jobid] = iter->second;
    runninglist.erase(iter);

    for (int i = 0; i < (int)completelist[jobid].count("RUNNODE"); i++)
    {
        it = nodelist.find(completelist[jobid]["RUNNODE"][i].get<string>());
        (it->second).setCPUcore((it->second).getnodeCPUcore() + completelist[jobid]["RUNNP"][i].get<int>());
    }

    runningtex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtocomplete(): Move job to complete queue  jobid = " << jobid << " content = " << completelist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtocomplete(): Move job to complete queue  jobid = " << jobid << " content = " << completelist[jobid].dump() << endl;
    }
}

void Monitor::setjobtofail(int jobid)
{
    map<int, json>::iterator iter;
    jobtex.lock();
    iter = joblist.find(jobid);
    if (iter == joblist.end())
    {
        jobtex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "FAIL";
    failtex.lock();
    faillist[jobid] = iter->second;
    failtex.unlock();
    joblist.erase(iter);
    jobtex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtofail(): Move job to fial queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtofail(): Move job to fail queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
    }
}

json Monitor::getjobstat()
{
    json result;
    jobtex.lock();
    int i = 0;
    for (map<int, json>::iterator it = joblist.begin(); it != joblist.end(); it++)
    {
        result["JOBID"][i] = (it->second)["JOBID"];
        if ((it->second).count("NODENEED") == 1)
        {
            result["NODENEED"][i] = (it->second)["NODENEED"];
        }
        else
        {
            result["NODENEED"][i] = 1;
        }
        if ((it->second).count("NPNEED") == 1)
        {
            result["NPNEED"][i] = (it->second)["NPNEED"];
        }
        else
        {
            result["NPNEED"][i] = 1;
        }
        i++;
    }
    jobtex.unlock();
    return result;
}

json Monitor::getjobinfo(int jobid)
{
    map<int, json>::iterator iter;
    json result;
    readytex.lock();
    iter = readylist.find(jobid);
    if (iter == readylist.end())
    {
        readytex.unlock();
        return NULL;
    }
    result = iter->second;
    readytex.unlock();
    return result;
}

void Monitor::setnodelist()
{
    ifstream nodes_fd;
    nodes_fd.open("node.con");
    if (nodes_fd.is_open())
    {
        string ip, port, name;
        int core;
        while (nodes_fd >> ip >> port >> name >> core)
        {
            Node node(ip, port, name, core);
            nodelist[name] = node;
        }
    }
    else
    {
        cout << "Server ---> main(): Error! node.con not found." << endl;
        exit(1);
    }
}

json Monitor::getnodelist()
{
    json result;
    int i = 0;
    for (map<string, Node>::iterator it = nodelist.begin(); it != nodelist.end(); it++)
    {
        result["NODES"][i] = it->second.getnodename();
        result["NPS"][i] = it->second.getnodeCPUcore();
        result["ONPS"][i] = it->second.getoriginalCPUcore();
        i++;
    }
    return result;
}

Node Monitor::getnodeinfo(string nodename)
{
    return nodelist[nodename];
}