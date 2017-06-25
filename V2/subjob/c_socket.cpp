#include"c_socket.hpp"
#include"iostream"
#include"cstdlib"
#include<unistd.h>
#include<string>
#include<cstdio>
#include<cstring>
using namespace std;

void c_socket::setConnection(string ip, string port){
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    cout << ip << " " << stoi(port) << endl;
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "subjob client socket creat error !" << endl;
        exit(1);
    }
}

void c_socket::connect2server(){
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "subjob client socket connect error !" << endl;
        exit(1);
    }
}

void c_socket::send(string msg){
    char *buf = new char[msg.size()];
    strcpy(buf,msg.c_str());
    write(sock_fd,buf,msg.size());
}

void c_socket::closeConnection(){
    close(sock_fd);
}