#include "Monitor.hpp"
#include "Node.hpp"
#include "Message.hpp"
#include "c_socket.hpp"
#include <ThreadPool.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <json.hpp>
#include "Server.hpp"

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
    newjob["SUBMITTIME"] = getcurrenttime();
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
    readytex.lock();
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
    (it->second).setCPUcore((it->second).getnodeCPUcore() - np);
    readytex.unlock();
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
    runningtex.lock();
    iter = readylist.find(jobid);
    if (iter == readylist.end())
    {
        runningtex.unlock();
        readytex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "RUNNING";
    (iter->second)["WAITTIME"] = getcurrenttime() - (iter->second)["SUBMITTIME"].get<long>();
    (iter->second)["MOTHERNODE"] = mothor;
    runninglist[jobid] = iter->second;
    readylist.erase(iter);
    runningtex.unlock();
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
    completetex.lock();
    iter = runninglist.find(jobid);
    if (iter == runninglist.end())
    {
        completetex.unlock();
        runningtex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "COMPLETE";
    (iter->second)["ENDTIME"] = getcurrenttime();
    completelist[jobid] = iter->second;
    runninglist.erase(iter);

    nodetex.lock();
    for (int i = 0; i < (int)completelist[jobid]["RUNNODE"].size(); i++)
    {
        it = nodelist.find(completelist[jobid]["RUNNODE"][i].get<string>());
        (it->second).setCPUcore((it->second).getnodeCPUcore() + completelist[jobid]["RUNNP"][i].get<int>());
    }
    nodetex.unlock();

    completetex.unlock();
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
    failtex.lock();
    iter = joblist.find(jobid);
    if (iter == joblist.end())
    {
        failtex.unlock();
        jobtex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "FAIL";
    faillist[jobid] = iter->second;
    joblist.erase(iter);
    failtex.unlock();
    jobtex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtofail(): Move job to fial queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtofail(): Move job to fail queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
    }
}

void Monitor::setjobtorunfail(int jobid)
{
    map<int, json>::iterator iter;
    runningtex.lock();
    failtex.lock();
    iter = runninglist.find(jobid);
    if (iter == runninglist.end())
    {
        failtex.unlock();
        runningtex.unlock();
        return;
    }
    (iter->second)["JOBSTAT"] = "RUNFAIL";
    faillist[jobid] = iter->second;
    runninglist.erase(iter);

    nodetex.lock();
    map<std::string, Node>::iterator it;
    for (int i = 0; i < (int)faillist[jobid]["RUNNODE"].size(); i++)
    {
        it = nodelist.find(faillist[jobid]["RUNNODE"][i].get<string>());
        (it->second).setCPUcore((it->second).getnodeCPUcore() + faillist[jobid]["RUNNP"][i].get<int>());
    }
    nodetex.unlock();

    failtex.unlock();
    runningtex.unlock();

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Server ---> Monitor setjobtorunfail(): Move job to fial queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
        else if (debug == 2)
            cout << "Server ---> Monitor setjobtorunfail(): Move job to fail queue  jobid = " << jobid << " content = " << faillist[jobid].dump() << endl;
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
        result["RUNTIME"][i] = (it->second)["RUNTIME"];
        result["SUBMITTIME"][i] = (it->second)["SUBMITTIME"];
        if ((it->second).count("NODENEED") == 1)
        {
            result["NODENEED"][i] = (it->second)["NODENEED"];
        }
        else
        {
            result["NODENEED"][i] = 0;
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
json Monitor::getrunstat()
{
    json result;
    int i = 0;
    runningtex.lock();
    for (map<int, json>::iterator it = runninglist.begin(); it != runninglist.end(); it++)
    {
        result["JOBID"][i] = (it->second)["JOBID"];
        result["RUNTIME"][i] = (it->second)["RUNTIME"];
        result["WAITTIME"][i] = (it->second)["WAITTIME"];
        result["SUBMITTIME"][i] = (it->second)["SUBMITTIME"];
        result["RUNNODE"][i] = (it->second)["RUNNODE"];
        result["RUNNP"][i] = (it->second)["RUNNP"];
        i++;
    }
    runningtex.unlock();
    return result;
}
json Monitor::getrunjobinfo(int jobid)
{
    map<int, json>::iterator iter;
    json result;
    runningtex.lock();
    iter = runninglist.find(jobid);
    if (iter == runninglist.end())
    {
        runningtex.unlock();
        return NULL;
    }
    result = iter->second;
    runningtex.unlock();
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
            c_socket cs;
            if (cs.setConnection(ip, port) == 0)
            {
                continue;
            }
            if (cs.connect2server() == 0)
            {
                continue;
            }

            Node node(ip, port, name, core);
            nodelist[name] = node;
            Message initmsg;
            initmsg.initMessage();
            Server *s;
            s = dynamic_cast<Server *>(observer);
            initmsg.msg["SERVERIP"] = s->getsvr_ip();
            initmsg.msg["SERVERPORT"] = s->getsvr_port();
            initmsg.encode_Header("server", "mom", "initmom");
            cs.send(initmsg.encode_Message());
            cs.closeConnection();
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
    nodetex.lock();
    for (map<string, Node>::iterator it = nodelist.begin(); it != nodelist.end(); it++)
    {
        result["NODES"][i] = it->second.getnodename();
        result["NPS"][i] = it->second.getnodeCPUcore();
        result["ONPS"][i] = it->second.getoriginalCPUcore();
        i++;
    }
    nodetex.unlock();
    return result;
}

Node Monitor::getnodeinfo(string nodename)
{
    return nodelist[nodename];
}

json Monitor::getall()
{
    std::map<int, json> all;

    jobtex.lock();
    readytex.lock();
    runningtex.lock();
    completetex.lock();
    failtex.lock();
    for (std::map<int, json>::iterator mi = joblist.begin(); mi != joblist.end(); mi++)
        all[mi->first] = mi->second;
    for (std::map<int, json>::iterator mi = readylist.begin(); mi != readylist.end(); mi++)
        all[mi->first] = mi->second;
    for (std::map<int, json>::iterator mi = runninglist.begin(); mi != runninglist.end(); mi++)
        all[mi->first] = mi->second;
    for (std::map<int, json>::iterator mi = completelist.begin(); mi != completelist.end(); mi++)
        all[mi->first] = mi->second;
    for (std::map<int, json>::iterator mi = faillist.begin(); mi != faillist.end(); mi++)
        all[mi->first] = mi->second;
    failtex.unlock();
    completetex.unlock();
    runningtex.unlock();
    readytex.unlock();
    jobtex.unlock();

    json result;
    int index = 0;
    for (std::map<int, json>::iterator mi = all.begin(); mi != all.end(); mi++, index++)
    {
        result["JOBID"][index] = (mi->second)["JOBID"];
        result["JOBSTAT"][index] = (mi->second)["JOBSTAT"];
        result["USER"][index] = (mi->second)["ENV"]["USER"];
        result["JOBNAME"][index] = (mi->second)["JOBNAME"];
        if ((mi->second)["JOBSTAT"].get<string>() == "RUNNING" || (mi->second)["JOBSTAT"].get<string>() == "READY" || (mi->second)["JOBSTAT"].get<string>() == "COMPLETE")
        {
            for (int i = 0; i < (int)(mi->second)["RUNNODE"].size(); i++)
            {
                result["RUNNODE"][index].push_back((mi->second)["RUNNODE"][i]);
                result["RUNNP"][index].push_back((mi->second)["RUNNP"][i]);
            }

            if ((mi->second)["JOBSTAT"].get<string>() == "RUNNING" || (mi->second)["JOBSTAT"].get<string>() == "COMPLETE")
                result["MOTHERNODE"][index] = (mi->second)["MOTHERNODE"];
        }
        if (debug == 2)
        {
            cout << "=========================================================================" << endl;
            cout << (mi->second).dump() << endl;
        }
    }

    return result;
}

long Monitor::getcurrenttime()
{
    long current_time;
    time_t sys_time;
    sys_time = time(NULL);
    current_time = difftime(sys_time, start_time);
    return current_time;
}

void Monitor::setstarttime()
{
    start_time = time(NULL);
}