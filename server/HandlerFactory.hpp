#ifndef _HANDLERFACTORY
#define _HANDLERFACTORY

#include "Handler.hpp"
#include "IHandler.hpp"
#include "s_socket.hpp"

class HandlerFactory
{
  public:
    static IHandler *getHandler(json, s_socket *);
};

#endif