#ifndef _JOBQUQUE
#define _JOBQUQUE

#include <mutex>
#include <vector>
#include <map>
#include <json.hpp>
#include "Observer.hpp"

using json = nlohmann::json;

class JobQueue{
    public:
        //static method that returns only instance of JobQueue
        static JobQueue *GetInstance();
        void attachmom(Observer *obs);

    private:
        static JobQueue *jobquque;
        Observer *observer;
        //holds one and only object of JobQueue
        JobQueue(){
            
        };

    public:
        void notitfyjobdone();
        void addjob(int,json);
        json getdonejob();
    private:
        std::map<int,json> jobqueue;
        std::mutex jobqueuetex;
};

#endif