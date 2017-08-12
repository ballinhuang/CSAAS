#include "MomHandlerFactory.hpp"
#include <iostream>
#include <fstream>
using namespace std;

extern int debug;
extern ofstream *debug_file;

IHandler *MomHandlerFactory::getHandler(json request, s_socket *s)
{

    //ERROR
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MomHandlerFactory getHandler(): ERROR!!"
                        << "----> ERROR request = " << endl
                        << request.dump() << endl;
        else if (debug == 2)
            cout << "MomHandlerFactory getHandler(): ERROR!!"
                 << "----> ERROR request = " << endl
                 << request.dump() << endl;
    }

    return NULL;
}