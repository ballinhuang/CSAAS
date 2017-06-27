#include "HandlerFactory.hpp"
#include <iostream>
#include <fstream>
using namespace std;

extern int debug;
extern ofstream *debug_file;


IHandler *HandlerFactory::getHandler(json request)
{

    //NewJobHandler
    if(request["SENDER"].get<std::string>()   == "subjob"       &&
       request["RECEIVER"].get<std::string>() == "server"       && 
       request["REQUEST"].get<std::string>()  == "newjob"){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return NewJobHandler." << endl;
            else if(debug == 2)
                cout << "HandlerFactory getHandler(): Return NewJobHandler." << endl;
        }
        return new NewJobHandler(request);
    }

    if(request["SENDER"].get<std::string>()   == "scheduler"    &&
       request["RECEIVER"].get<std::string>() == "server"       && 
       request["REQUEST"].get<std::string>()  == "runjob"){
        if(debug > 0){
            if(debug == 1)
                *debug_file << "HandlerFactory getHandler(): Return RunJobHandler." << endl;
            else if(debug == 2)
                cout << "HandlerFactory getHandler(): Return RunJobHandler." << endl;
        }
        return new RunJobHandler(request);
    }

    //ERROR
    if(debug > 0){
        if(debug == 1)
            *debug_file << "HandlerFactory getHandler(): ERROR!!" << "----> ERROR request = " << endl << request.dump() << endl;
        else if(debug == 2)
            cout << "HandlerFactory getHandler(): ERROR!!" << "----> ERROR request = " << endl << request.dump() << endl;
    }
    
}