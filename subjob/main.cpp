#include <iostream>
#include <fstream>
#include <string>

#include "Message.hpp"
#include "cc_socket.hpp"
#include "subjob_service.hpp"

using namespace std;

int main(int argc, char **argv){

    string script_name;
    if(argc > 0 && argv[1] != NULL){
        script_name = std::string(argv[1]);
    }
    else{
        cout << "Subjob ---> main(): Error! No input script file." << endl;
        exit(1);
    }
    
    Message newjob;
    newjob.initMessage();

    subjob_service srv;
    srv.creatjob(&newjob, script_name);

    string ip,port;
    ifstream f;
    f.open("subjob.con");
    if (f.is_open()){
        f >> ip;
        f >> port;
    }
    else{
        cout << "Subjob ---> main(): Error! subjob.con not found." << endl;
        exit(1);
    }
    
    cc_socket csock;
    if(csock.setConnection(ip,port) != 1 ){
        exit(1);
    }
    if(csock.connect2server() != 1 ){
        exit(1);
    }

    csock.send(newjob.encode_Message());
    csock.closeConnection();

}