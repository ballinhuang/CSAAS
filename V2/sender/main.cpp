#include<iostream>
#include"Message.hpp"
#include"cc_socket.hpp"
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

    cc_socket csock;
    csock.setConnection("127.0.0.1","5002");
    csock.connect2server();
    csock.send(msg.encode_Message());
    //cout << csock.receive() << endl;
    csock.closeConnection();
}