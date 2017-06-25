#ifndef _MONITOR
#define _MONITOR

#include <mutex>
#include <vector>
#include "Observer.hpp"



class Monitor{
    public:
        //static method that returns only instance of Moniter
        static Monitor *GetInstance();
        void attachserver(Observer *obs);

    private:
        static Monitor *monitor;
        Observer *observer;
        //holds one and only object of Moniter
        Monitor(){};// private constructor
    //test
    std::vector<int> a;
    std::mutex atx;
    public:
    void foo(int);
	void zoo(int);
};

#endif