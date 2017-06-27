#include "HandlerFactory.hpp"
#include <iostream>
#include <fstream>
using namespace std;

extern bool debug;
extern ofstream *debug_file;


IHandler *HandlerFactory::getHandler(json request)
{
    if(debug){
        *debug_file << "HandlerFactory getHandler(): SENDER = " + request["SENDER"].get<std::string>() 
        + " RECEIVER = " + request["RECEIVER"].get<std::string>() 
        + " REQUEST = " + request["REQUEST"].get<std::string>() << endl;
    }
        if(request["SENDER"].get<std::string>() == "subjob"   &&
           request["RECEIVER"].get<std::string>() == "server" && 
           request["REQUEST"].get<std::string>() == "newjob"){
            if(debug){
                *debug_file << "HandlerFactory getHandler(): Return NewJobHandler." << endl;
            }
            return new NewJobHandler(request);
        }
    if(debug){
        *debug_file << "ERROR request:" << endl << request.dump() << endl;
        *debug_file << "HandlerFactory getHandler(): ERROR!!" << endl;
    }
}