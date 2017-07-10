#ifndef _MOMHANDLERFACTORY
#define _MOMHANDLERFACTORY

#include "MomHandler.hpp"
#include "IHandler.hpp"
#include "s_socket.hpp"

class MomHandlerFactory{
    public:
    static IHandler *getHandler(json,s_socket*);
};

#endif