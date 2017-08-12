#include "JobQueue.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <json.hpp>
#include <unistd.h>
#include <signal.h>

using namespace std;
using json = nlohmann::json;
extern mutex jobqueue_mtx;

extern int debug;
extern ofstream *debug_file;

JobQueue *JobQueue::GetInstance()
{
    if (jobquque == NULL)
    {
        jobqueue_mtx.lock();
        if (jobquque == NULL)
        {
            jobquque = new JobQueue();
        }
        jobqueue_mtx.unlock();
    }
    return jobquque;
}

void JobQueue::attachmom(Observer *obs)
{
    observer = obs;
    obs->attach_success();
}

void JobQueue::notitfyjobdone()
{
    if (!i_am_jobstarter)
    {
        observer->notify(0);
    }
}

void JobQueue::addjob(int jobid, json job)
{
    jobqueuetex.lock();
    jobqueue[jobid] = job;
    jobqueuetex.unlock();
}

json JobQueue::getdonejob()
{
    json list;
    jobqueuetex.lock();
    for (map<int, json>::iterator it = jobqueue.begin(); it != jobqueue.end(); it++)
    {
        pid_t pid = (it->second)["PID"].get<int>();
        if (pid == 0)
        {
            list["COMPLETEJOB"].push_back(it->first);
        }
    }
    for (int i = 0; i < (int)list["COMPLETEJOB"].size(); i++)
    {
        map<int, json>::iterator iter;
        iter = jobqueue.find(list["COMPLETEJOB"][i].get<int>());
        jobqueue.erase(iter);
    }
    jobqueuetex.unlock();
    return list;
}