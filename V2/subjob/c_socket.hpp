#ifndef _C_SOCKET
#define _C_SOCKET

#include<iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class c_socket{

    public:
    void setConnection(std::string,std::string);
    void connect2server();
    void send(std::string);
    void closeConnection();

    private:
    struct sockaddr_in server_addr;
    int sock_fd;
};

#endif