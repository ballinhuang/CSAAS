#include "MomHandler.hpp"
#include <json.hpp>
#include "Message.hpp"
using json = nlohmann::json;
using namespace std;

extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

RunJobHandler::RunJobHandler(json job, s_socket *socket){
    req_run_job = job;
    s = socket;
}

void RunJobHandler::handle(){

}

//NewJobHandler end
