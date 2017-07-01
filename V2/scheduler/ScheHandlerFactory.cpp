#include "ScheHandlerFactory.hpp"
#include <iostream>

using namespace std;
extern int debug;
extern ofstream *debug_file;

IScheHandler ScheHandlerFactory::getScheHandler(string mode, cc_socket *socket){
    if(mode == "FIFO"){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "Scheduler ---> ScheHandlerFactory getScheHandler(): Return ScheHandlerFactory." << endl;
            else if(debug == 2)
                cout << "Scheduler ---> ScheHandlerFactory getScheHandler(): Return ScheHandlerFactory." << endl;
        }
        return new FIFOScheHandler(socket);
    }

    if(debug > 0){
        if(debug == 1)
            *debug_file << "Scheduler ---> ScheHandlerFactory getScheHandler(): ERROR!" << endl;
        else if(debug == 2)
            cout << "Scheduler ---> ScheHandlerFactory getScheHandler(): ERROR!" << endl;
    }
    return NULL;
}