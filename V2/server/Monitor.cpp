#include "Monitor.hpp"
#include "iostream"
#include "cstdio"

using namespace std;
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


//test
void Monitor::foo(int s){
    atx.lock();
	a.push_back(1);
    observer->notify();
	atx.unlock();
	printf("job %d call foo.\n",s);
}

void Monitor::zoo(int s){
    atx.lock();
	printf("job %d call zoo : value a= %d\n",s,a.size());
    atx.unlock();
}