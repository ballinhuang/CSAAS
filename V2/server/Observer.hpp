#ifndef _OBSERVER
#define _OBSERVER

class Observer
{
	public:
		virtual void notify() = 0;
        virtual void attach_success()=0;
};

#endif