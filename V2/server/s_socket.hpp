#ifndef _S_SOCKET
#define _S_SOCKET

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class s_socket{
    public:
    void setConnection(std::string,std::string);
    void acceptClinet();
    std::string readmessage();
    void closeConnection();
    //void getClientaddr();

    private:
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;	
    int sock_fd,client_sock_fd;
};

#endif