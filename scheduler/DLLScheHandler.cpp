#include "DLLScheHandler.hpp"

#include <fstream>
#include "Message.hpp"

extern int debug;
extern ofstream *debug_file;

using json = nlohmann::json;

DLLScheHandler::DLLScheHandler(IScheduler *scheduler, cc_socket *socket)
{
    this.scheduler = scheduler;
    this.socket = socket;
}

void DLLScheHandler::handleschedule()
{
    //get info frome server
    //call scheduler.schedule(waitq,runq,time,nodes);
    //encode to Message and send to server
}