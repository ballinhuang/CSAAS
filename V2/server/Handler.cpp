#include "Handler.hpp"
#include <json.hpp>

using json = nlohmann::json;
using namespace std;

extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

NewJobHandler::NewJobHandler(json job){
    new_job = job;
}

void NewJobHandler::handle(){
    Monitor::GetInstance()->addjob(new_job);
}

//NewJobHandler end

//RunJobHandler start

RunJobHandler::RunJobHandler(json req_run){
    req_runjob = req_run;
}

void RunJobHandler::handle(){
    Monitor::GetInstance()->notitfyschedualfinish();
}

//RunJobHandler end