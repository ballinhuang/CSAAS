#ifndef _JOBQUQUE
#define _JOBQUQUE

#include <mutex>
#include <vector>
#include <map>
#include <json.hpp>
#include "Observer.hpp"

using json = nlohmann::json;

class JobQueue
{
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
    void addjob(int, int);
    void erasedownjob();
    void killjob(int);
    bool i_am_jobstarter = false;

  private:
    std::map<int, int> jobqueue;
    std::mutex jobqueuetex;
};

#endif