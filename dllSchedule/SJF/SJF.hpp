#ifndef SJF_HPP
#define SJF_HPP

#include "IScheduler.hpp"

class SJF : public IScheduler {
    public:
        SJF();
        deque<Job> schedule(deque<Job>, deque<Job>, long, deque<Node>);

    private:
        static bool cmpRuntime(Job, Job);
        void setNp();
        bool assignedNodeSchedule(Job, deque<Job> &);
        bool arbitraryNodeSchedule(Job, deque<Job> &);

        int allNp;
        int remainingNp;
        long currentTime;
        deque<Job> waitQ;
        deque<Job> runQ;
        deque<Node> nodeQ;
};

#endif