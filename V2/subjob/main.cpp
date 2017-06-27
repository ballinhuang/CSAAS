#include<iostream>
#include"Message.hpp"
#include"c_socket.hpp"
#include"subjob_service.hpp"
#include <fstream>
#include <string>

using namespace std;
int main(int argc, char **argv){

    string script_name;
    
    if(argc > 0 && argv[1] != NULL){
        script_name = std::string(argv[1]);
    }
    else{
        std::cout << "err: No input script file.\n";
        exit(1);
    }
    
    Message newjob;
    newjob.initMessage();
    
    subjob_service srv;
    srv.creatjob(&newjob, script_name);

    ifstream f("subjob.con");
    string ip,port;
    f >> ip;
    f >> port;

    c_socket csock;
    csock.setConnection(ip,port);
    csock.connect2server();
    csock.send(newjob.encode_Message());
    csock.closeConnection();

}