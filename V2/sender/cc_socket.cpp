#include"cc_socket.hpp"
#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<string>
#include<cstdio>
#include<cstring>
using namespace std;

void cc_socket::setConnection(string ip, string port){
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    cout << ip << " " << stoi(port) << endl;
    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "subjob client socket creat error !" << endl;
        exit(1);
    }
}

void cc_socket::connect2server(){
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "subjob client socket connect error !" << endl;
        exit(1);
    }
}

void cc_socket::send(string msg){
    sendhendshack(msg.size());
    char *buf = new char[msg.size()];
    strcpy(buf,msg.c_str());
    write(sock_fd,buf,msg.size());
}

void cc_socket::closeConnection(){
    close(sock_fd);
}

void cc_socket::sendhendshack(int size){
    char num[10];
    sprintf(num,"%d",size);
    write(sock_fd,num,sizeof(num));
}

string cc_socket::receive(){
    int size = receivehendshack();
    string result = "";
    char *buf = new char[size+1];
    memset(buf, 0, size+1);
    read(sock_fd,buf,(size_t)size);
    result = buf;
    return result;
}

int cc_socket::receivehendshack(){
    char num[10];
    read(sock_fd,num,sizeof(num));
    int size = atoi(num);
    return size;
}