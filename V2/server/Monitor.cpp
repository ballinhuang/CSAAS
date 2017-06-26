#include "Monitor.hpp"
#include "iostream"
#include "cstdio"
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
            cout << "creat moniter\n";
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
    observer->notify();
}

void Monitor::addjob(json newjob){
    jobtex.lock();
    newjob["JOBID"] = jobcount;
    jobcount++;
    cout << newjob.dump();
    joblist.push_back(newjob);
    notitfynewjob();
    jobtex.unlock();
}