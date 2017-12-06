#include "SJF.hpp"
#include <algorithm>

IScheduler* getInstance() {
    return new SJF;
}

SJF::SJF() {}

deque<Job> SJF::schedule(deque<Job> wait, deque<Job> run, long currentT, deque<Node> node) {
    waitQ = wait;
    runQ = run;
    nodeQ = node;
    currentTime = currentT;
    sort(waitQ.begin(), waitQ.end(), cmpRuntime);
    setNp();

    deque<Job> resultQ;

    for(deque<Job>::iterator di = waitQ.begin(); di != waitQ.end(); di++)
        if(di->getRequiredNode() > 1) {
            if(!assignedNodeSchedule(*di, resultQ))
                break;
        }
        else {
            if(!arbitraryNodeSchedule(*di, resultQ))
                break;
        }

    return resultQ;
}

bool SJF::cmpRuntime(Job a, Job b) {
    if(a.getRun() != b.getRun())
        return a.getRun() < b.getRun();
    else if(a.getSubmit() != b.getSubmit())
        return a.getSubmit() < b.getSubmit();

    return a.getId() < b.getId();
}

void SJF::setNp() {
    allNp = 0;
    remainingNp = 0;

    for(deque<Node>::iterator di = nodeQ.begin(); di != nodeQ.end(); di++) {
        allNp += di->getOnp();
        remainingNp += di->getNp();
    }
}

bool SJF::assignedNodeSchedule(Job j, deque<Job> &resultQ) {
    deque<Job> tempQ;

    for(deque<Node>::iterator di = nodeQ.begin(); di != nodeQ.end() && tempQ.size() < j.getRequiredNode(); di++)
        if(di->getNp() >= j.getNp()) {
            Job doJob(j.getId(), j.getNp(), di->getNodeName());
            tempQ.push_back(doJob);
            di->setNp(di->getNp() - j.getNp());
        }

    if(tempQ.size() == j.getRequiredNode()) {
        for(deque<Job>::iterator di = tempQ.begin(); di != tempQ.end(); di++)
            resultQ.push_back(*di);

        return true;
    }

    int counter = 0;

    for(deque<Node>::iterator di = nodeQ.begin(); di != nodeQ.end() && counter < j.getRequiredNode(); di++)
        if(di->getOnp() >= j.getNp())
            counter++;

    if(counter < j.getRequiredNode()) {
        Job doJob(j.getId(), -1, "FAIL");
        resultQ.push_back(doJob);

        return true;
    }
    
    return false;
}

bool SJF::arbitraryNodeSchedule(Job j, deque<Job> &resultQ) {
    int jobNp = j.getNp();

    if(jobNp <= remainingNp) {
        for(deque<Node>::iterator di = nodeQ.begin(); di != nodeQ.end() && jobNp > 0; di++)
            if(di->getNp() == 0)
                continue;
            else if(jobNp > di->getNp()) {
                Job doJob(j.getId(), di->getNp(), di->getNodeName());
                resultQ.push_back(doJob);
                jobNp -= di->getNp();
                di->setNp(0);
            }
            else {
                Job doJob(j.getId(), jobNp, di->getNodeName());
                resultQ.push_back(doJob);
                di->setNp(di->getNp() - jobNp);
                jobNp = 0;
            }
    }
    else if(jobNp > allNp) {
        Job doJob(j.getId(), -1, "FAIL");
        resultQ.push_back(doJob);
    }
    else
        return false;

    return true;
}