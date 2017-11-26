#include "ScheHandlerFactory.hpp"
#include <iostream>
#include <fstream>
#include "FIFOScheHandler.hpp"
#include "DLLScheHandler.hpp"
using namespace std;
extern int debug;
extern ofstream *debug_file;

IScheHandler *ScheHandlerFactory::getScheHandler(IScheduler *scheduler, cc_socket *socket)
{
    if (scheduler == NULL)
    {
        return new FIFOScheHandler(socket);
    }

    else
    {
        return new DLLScheHandler(scheduler, socket);
    }

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "Scheduler ---> ScheHandlerFactory getScheHandler(): ERROR!" << endl;
        else if (debug == 2)
            cout << "Scheduler ---> ScheHandlerFactory getScheHandler(): ERROR!" << endl;
    }
    return NULL;
}