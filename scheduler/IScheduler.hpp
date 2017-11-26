#ifndef _ISCHEDULER
#define _ISCHEDULER

class IScheduler
{
  public:
    //schedule(waitq,runq,time,nodes)
    //creat_object()
    virtual void schedule() = 0;
};

#endif