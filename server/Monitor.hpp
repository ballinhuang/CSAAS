#ifndef _MONITOR
#define _MONITOR

#include <mutex>
#include <vector>
#include <map>
#include <json.hpp>
#include "Node.hpp"
#include "Observer.hpp"
#include <ThreadPool.h>

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
            //jobtex.unlock();
        };// private constructor
    
    //data
    public:
        void addjob(json);
        void notitfyschedualfinish();
        json getjobstat();
        void setnodelist();
        json getnodelist();
        void setjobtoready(int,std::string);
        void setjobtorunning(int,std::string);
        Node getnodeinfo(std::string);
        json getjobinfo(int);
    private:
        std::map<std::string,Node> nodelist;
        std::map<int,json> joblist;
        std::map<int,json> readylist;
        std::map<int,json> runninglist;
        std::map<int,json> completelist;
        std::mutex jobtex;
        std::mutex readytex;
        std::mutex runningtex;
        int jobcount = 0; 
        void notitfynewjob();
};

#endif