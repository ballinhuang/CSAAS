#ifndef SJF_HPP
#define SJF_HPP

#include "IScheduler.hpp"

class SJF : public IScheduler {
    public:
        SJF();
        deque<Job> schedule(deque<Job>, deque<Job>, long, deque<Node>);

    private:
        static bool cmpRuntime(Job, Job);
        void setNp(deque<Node> &);
        void doSchedule(deque<Job> &, deque<Job> &, deque<Job> &);
        void assignNode(deque<Job> &, deque<Node> &);
        void addFailJob(deque<Job> &);

        int allNp;
        int remainingNp;
        deque<Job> result;
};

#endif