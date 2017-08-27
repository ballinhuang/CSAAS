#ifndef _SCHEHANDLERFACTORY
#define _SCHEHANDLERFACTORY

#include "IScheHandler.hpp"
#include <iostream>
#include "cc_socket.hpp"

class ScheHandlerFactory{
    public:
    static IScheHandler *getScheHandler(std::string, cc_socket*);
};

#endif