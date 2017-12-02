#include "SJF.hpp"
#include <algorithm>

IScheduler* getInstance() {
    return new SJF;
}

SJF::SJF() {}

deque<Job> SJF::schedule(deque<Job> waitQ, deque<Job> runQ, long currentTime, deque<Node> nodeQ) {
    sort(waitQ.begin(), waitQ.end(), cmpRuntime);
    setNp(nodeQ);
    result.clear();

    deque<Job> doJob, failJob;
    doSchedule(doJob, failJob, waitQ);
    assignNode(doJob, nodeQ);
    addFailJob(failJob);

    return result;
}

bool SJF::cmpRuntime(Job a, Job b) {
    if(a.getRun() != b.getRun())
        return a.getRun() < b.getRun();
    else if(a.getSubmit() != b.getSubmit())
        return a.getSubmit() < b.getSubmit();

    return a.getId() < b.getId();
}

void SJF::setNp(deque<Node> &node) {
    allNp = 0;
    remainingNp = 0;

    for(deque<Node>::iterator di = node.begin(); di != node.end(); di++) {
        allNp += di->getOnp();
        remainingNp += di->getNp();
    }
}

void SJF::doSchedule(deque<Job> &doJob, deque<Job> &failJob, deque<Job> &wait) {
    for(deque<Job>::iterator di = wait.begin(); di != wait.end(); di++) {
        if(di->getNp() <= remainingNp) {
            doJob.push_back(*di);
            remainingNp -= di->getNp();
        }
        else if(di->getNp() > allNp)
            failJob.push_back(*di);
        else
            break;
    }
}

void SJF::assignNode(deque<Job> &doJob, deque<Node> &node) {
    for(deque<Job>::iterator di = doJob.begin(); di != doJob.end(); di++) {
        int tempNp = di->getNp();

        for(deque<Node>::iterator ni = node.begin(); ni != node.end() && tempNp > 0; ni++)
            if(ni->getNp() == 0) {
                node.erase(ni);
                ni--;
            }
            else if(tempNp <= ni->getNp()) {
                Job j(di->getId(), tempNp, ni->getNodeName());
                result.push_back(j);
                ni->setNp(ni->getNp() - tempNp);
                tempNp = 0;
            }
            else {
                Job j(di->getId(), ni->getNp(), ni->getNodeName());
                result.push_back(j);
                tempNp -= ni->getNp();
                ni->setNp(0);
            }
    }
}

void SJF::addFailJob(deque<Job> &failJob) {
    for(deque<Job>::iterator di = failJob.begin(); di != failJob.end(); di++) {
        Job j(di->getId(), -1, "FAIL");
        result.push_back(j);
    }
}