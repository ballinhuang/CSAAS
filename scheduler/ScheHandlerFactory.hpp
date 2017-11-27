#ifndef _SCHEHANDLERFACTORY
#define _SCHEHANDLERFACTORY
#include <iostream>
#include "IScheHandler.hpp"
#include "cc_socket.hpp"
#include "IScheduler.hpp"

class ScheHandlerFactory
{
  public:
    static IScheHandler *getScheHandler(IScheduler *, cc_socket *);
};

#endif