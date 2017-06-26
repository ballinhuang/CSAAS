#ifndef _MONITOR
#define _MONITOR

#include <mutex>
#include <vector>
#include <json.hpp>
#include "Observer.hpp"

using json = nlohmann::json;

class Monitor{
    public:
        //static method that returns only instance of Moniter
        static Monitor *GetInstance();
        void attachserver(Observer *obs);

    private:
        static Monitor *monitor;
        Observer *observer;
        //holds one and only object of Moniter
        Monitor(){
            jobtex.unlock();
        };// private constructor
    
    //data
    public:
        void addjob(json);
    private:
        std::vector<json> joblist;
        std::mutex jobtex;
        int jobcount = 0; 
        void notitfynewjob();
};

#endif