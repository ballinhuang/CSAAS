#ifndef _HANDLER
#define _HANDLER

#include "IHandler.hpp"
#include <json.hpp>
#include "Monitor.hpp"

using json = nlohmann::json;

class NewJobHandler : public IHandler{
    public:
    NewJobHandler(json);
    void handle();
    private:
    json new_job;
};


class RunJobHandler : public IHandler{
    public:
    RunJobHandler(json);
    void handle();
    private:
    json req_runjob;
};

#endif