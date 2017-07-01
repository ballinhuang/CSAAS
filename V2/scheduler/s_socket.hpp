#ifndef _S_SOCKET
#define _S_SOCKET

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

class s_socket{
    public:
    int setConnection(std::string,std::string);
    int acceptClinet();
    std::string readmessage();
    void closeConnection();
    void closebind();
    std::string getClientIP();
    std::string getClientPort();
    void setacceptreuse();
    void sendmessage(std::string);
    private:
    int receivehendshack();
    void sendhendshack(int size);
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;	
    int sock_fd,conn_port;
};

#endif