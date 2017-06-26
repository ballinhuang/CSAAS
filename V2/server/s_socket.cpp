#include"s_socket.hpp"
#include<iostream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<cstdio>
#include <unistd.h>
using namespace std;

void s_socket::setConnection(string ip, string port){

    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    cout << port;
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Server socket creat error !" << endl;
        return;
    }

    if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "server bind error !" << endl;
        return;
    }

    if(listen(sock_fd, 1) == -1) {
        cout << "server listen error !" << endl;
        return;
    }
}

void s_socket::acceptClinet(){
    memset(&client_addr, '\0', sizeof(struct sockaddr_in));
    int sin_size = sizeof(struct sockaddr_in);
    if((client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1){
        return;
    }
}

string s_socket::readmessage(){
    string result = "";
    char buf[100];
    memset(buf, '\0', 100);
    while(read(client_sock_fd,buf,100)){
        result += buf;
        memset(buf, '\0', 100);
    }
    return result;
}

void s_socket::closeConnection(){
    close(sock_fd);
}