#ifndef _C_SOCKET
#define _C_SOCKET

#include<iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class c_socket{

    public:
    int setConnection(std::string,std::string);
    int connect2server();
    void send(std::string);
    void closeConnection();
    void sendhendshack(int);
    private:
    struct sockaddr_in server_addr;
    int sock_fd;
};

#endif