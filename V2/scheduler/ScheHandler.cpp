#include "ScheHandler.hpp"

using namespace std;
extern int debug;
extern ofstream *debug_file;
//FIFOScheHandler start

FIFOScheHandler::FIFOScheHandler(cc_socket *sock){
    socket = sock;
}

void FIFOScheHandler::handleschedule(){
    if(debug > 0){
        if(debug == 1)
            *debug_file << "Scheduler ---> FIFOScheHandler handleschedule(): Start handle schedule." << endl;
        else if(debug == 2)
            cout << "Scheduler ---> FIFOScheHandler handleschedule(): Start handle schedule." << endl;
    }
}

//FIFOScheHandler end