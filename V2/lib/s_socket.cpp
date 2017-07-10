#include"s_socket.hpp"
#include<iostream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<cstdio>
#include <unistd.h>
using namespace std;

int s_socket::setConnection(string ip, string port){

    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;

    if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "Server socket creat error !" << endl;
        return 0;
    }

    int on = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        close(sock_fd);
        sock_fd = -3;
        return 0;
    }
    /*
    struct linger l_delay;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
    {
        close(sock_fd);
        sock_fd = -4;
    }
    */

    if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "Server socket bind error !" << endl;
        return 0;
    }

    if(listen(sock_fd, 256) == -1) {
        cout << "Server socket listen error !" << endl;
        return 0;
    }
    return 1;
}

int s_socket::acceptClinet(){
    memset(&client_addr, '\0', sizeof(struct sockaddr_in));
    int sin_size = sizeof(struct sockaddr_in);
    if((conn_port = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1){
        return 0;
    }
    return 1;
}

string s_socket::readmessage(){
    int size = receivehendshack();
    string result = "";
    char *buf = (char*)malloc(sizeof(char) * (size+1));
    memset(buf,0,size+1);
    read(conn_port,buf,(size_t)size);
    result = buf;
    memset(buf,0,size+1);
    free(buf);
    return result;
}

int s_socket::receivehendshack(){
    char num[10];
    read(conn_port,num,sizeof(num));
    int size = atoi(num);
    return size;
}

void s_socket::sendmessage(string msg){
    sendhendshack(msg.size());
    char *buf = (char*)malloc(sizeof(char) * (msg.size()+1));
    memset(buf,0,msg.size()+1);
    strcpy(buf,msg.c_str());
    write(conn_port,buf,msg.size());
    memset(buf,0,msg.size()+1);
    free(buf);
}

void s_socket::sendhendshack(int size){
    char num[10];
    sprintf(num,"%d",size);
    write(conn_port,num,sizeof(num));
}

void s_socket::closebind(){
    close(sock_fd);
}

void s_socket::closeConnection(){
    close(conn_port);
}

void s_socket::setacceptreuse(){
    int sockoptval = 1;
    setsockopt(conn_port, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval));
}
/*
string s_socket::getClientIP(){
    return to_string(client_addr.sin_addr.s_addr);
}

string s_socket::getClientPort(){
    return to_string(client_addr.sin_port);
}
*/