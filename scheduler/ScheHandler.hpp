#ifndef _SCHEHANDLER
#define _SCHEHANDLER

#include "IScheHandler.hpp"
#include "cc_socket.hpp"

class FIFOScheHandler : public IScheHandler{
    public:
    FIFOScheHandler(cc_socket*);
    void handleschedule();
    private:
    cc_socket *socket;
};

#endif