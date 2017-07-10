#ifndef _CC_SOCKET
#define _CC_SOCKET

#include<iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class cc_socket{

    public:
    void setConnection(std::string,std::string);
    void connect2server();
    void send(std::string);
    void closeConnection();
    std::string receive();
    
    private:
    int receivehendshack();
    void sendhendshack(int);
    struct sockaddr_in server_addr;
    int sock_fd;
};

#endif