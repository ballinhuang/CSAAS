#include<iostream>
#include"Message.hpp"
#include"c_socket.hpp"
#include <fstream>
#include <string>

using namespace std;
int main(int argc, char **argv){

    string sender,receiver,request;
    if(argc > 3){
        sender = std::string(argv[1]);
        receiver = std::string(argv[2]);
        request = std::string(argv[3]);
    }
    else{
        cout << "error";
        exit(1);
    }

    Message msg;
    msg.initMessage();
    msg.encode_Header(sender,receiver,request);

    c_socket csock;
    csock.setConnection("127.0.0.1","5001");
    csock.connect2server();
    csock.send(msg.encode_Message());
    csock.closeConnection();
}