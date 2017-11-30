#ifndef DLLSCHEHANDLER_HPP
#define DLLSCHEHANDLER_HPP

#include "IScheHandler.hpp"
#include "cc_socket.hpp"
#include "IScheduler.hpp"
#include "Job.hpp"
#include "Message.hpp"
#include <deque>

extern int debug;

class DLLScheHandler : public IScheHandler
{
    public:
        DLLScheHandler(IScheduler *, cc_socket *);
        void handleschedule();

    private:
        json getInfo(string);
        void setNodeQ();
        void setWaitQ();
        void setRunQ();
        void setTimer();
        void sendResult(deque<Job>);

        IScheduler *scheduler;
        cc_socket *socket;
        deque<Job> waitQ;
        deque<Job> runQ;
        deque<Node> nodeQ;
        long timer;
};

#endif