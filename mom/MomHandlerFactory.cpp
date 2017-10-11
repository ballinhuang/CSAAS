#include "MomHandlerFactory.hpp"
#include <iostream>
#include <fstream>
using namespace std;

extern int debug;
extern ofstream *debug_file;

IHandler *MomHandlerFactory::getHandler(json request, s_socket *s)
{
    if (request["SENDER"].get<std::string>() == "server" &&
        request["RECEIVER"].get<std::string>() == "mom" &&
        request["REQUEST"].get<std::string>() == "killjob")
    {
        if (debug > 0)
        {
            if (debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return KillJobHandler." << endl;
            else if (debug == 2)
                cout << "HandlerFactory getHandler(): Return KillJobHandler." << endl;
        }
        return new KillJobHandler(request, s);
    }

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