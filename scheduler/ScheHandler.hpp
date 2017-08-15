#ifndef _SCHEHANDLER
#define _SCHEHANDLER

#include "IScheHandler.hpp"
#include "cc_socket.hpp"
#include "Message.hpp"

class FIFOScheHandler : public IScheHandler{
    public:
    FIFOScheHandler(cc_socket*);
    void handleschedule();
    bool handleUnsignNode(Message &, json &, json &, int);
    bool handleSingleNode(Message &, json &, json &, int);
    bool handleMultiNode(Message &, json &, json &, int);

    private:
    cc_socket *socket;
};

#endif