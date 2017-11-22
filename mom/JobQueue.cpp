#include "JobQueue.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <json.hpp>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

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

void JobQueue::addjob(int jobid, int pid)
{
    jobqueuetex.lock();
    jobqueue[jobid] = pid;
    //cout << "ADD " << jobid << endl;
    jobqueuetex.unlock();
}

void JobQueue::erasedownjob()
{
    json list;
    jobqueuetex.lock();
    for (map<int, int>::iterator it = jobqueue.begin(); it != jobqueue.end(); it++)
    {
        pid_t pid = (it->second);
        if (kill(pid, 0) != 0)
        {
            list["COMPLETEJOB"].push_back(it->first);
        }
    }
    for (int i = 0; i < (int)list["COMPLETEJOB"].size(); i++)
    {
        map<int, int>::iterator iter;
        iter = jobqueue.find(list["COMPLETEJOB"][i].get<int>());
        //cout << "erase " << iter->first << endl;
        jobqueue.erase(iter);
    }

    for (map<int, int>::iterator it = jobqueue.begin(); it != jobqueue.end(); it++)
    {

        //cout << "all" << it->first << " " << it->second << endl;
    }
    jobqueuetex.unlock();
    return;
}

void JobQueue::killjob(int jobid)
{
    jobqueuetex.lock();
    if (jobqueue.find(jobid) != jobqueue.end())
    {
        kill(jobqueue[jobid], SIGTERM);
        map<int, int>::iterator iter;
        iter = jobqueue.find(jobid);
        jobqueue.erase(iter);
        //cout << "Kill " << jobid << endl;
    }
    jobqueuetex.unlock();
}