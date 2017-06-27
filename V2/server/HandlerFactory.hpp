#ifndef _HANDLERFACTORY
#define _HANDLERFACTORY

#include "Handler.hpp"
#include "IHandler.hpp"

class HandlerFactory{
    public:
    static IHandler *getHandler(json);
};

#endif