#include"c_socket.hpp"
#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<string>
#include<cstdio>
#include<cstring>
using namespace std;

#include <fstream>
extern int debug;
extern ofstream *debug_file;

int c_socket::setConnection(string ip, string port){
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        return 0;
    }
    return 1;
}

int c_socket::connect2server(){
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        return 0;
    }
    return 1;
}

void c_socket::sendhendshack(int size){
    char num[10];
    sprintf(num,"%d",size);
    write(sock_fd,num,sizeof(num));
}

void c_socket::send(string msg){
    sendhendshack(msg.size());
    char *buf = new char[msg.size()];
    strcpy(buf,msg.c_str());
    write(sock_fd,buf,msg.size());
}

void c_socket::closeConnection(){
    close(sock_fd);
}