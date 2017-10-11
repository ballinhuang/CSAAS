
#include "MomHandler.hpp"
#include <json.hpp>
#include "JobQueue.hpp"
#include "s_socket.hpp"

using json = nlohmann::json;

KillJobHandler::KillJobHandler(json job, s_socket *socket)
{
    kill_job = job;
    s = socket;
}

void KillJobHandler::handle()
{
    s->closeConnection();
    if (kill_job.count("JOBID") == 1)
    {
        JobQueue::GetInstance()->killjob(kill_job["JOBID"].get<int>());
    }
}