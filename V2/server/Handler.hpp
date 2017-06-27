#ifndef _HANDLER
#define _HANDLER

#include "IHandler.hpp"
#include <json.hpp>
#include "Monitor.hpp"
using json = nlohmann::json;

class NewJobHandler : public IHandler{
    public:
    NewJobHandler(json job){
        newjob = job;
    }
    void handle(){
        Monitor::GetInstance()->addjob(newjob);
    }
    private:
    json newjob;
};

#endif