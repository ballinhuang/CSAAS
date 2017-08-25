#include "cc_socket.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <cstring>
using namespace std;

int cc_socket::setConnection(string ip, string port)
{
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return 0;
    }
    return 1;
}

int cc_socket::connect2server()
{
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        return 0;
    }
    return 1;
}

void cc_socket::send(string msg)
{
    sendhendshack(msg.size());
    char *buf = (char *)malloc(sizeof(char) * (msg.size() + 1));
    memset(buf, 0, msg.size() + 1);
    strcpy(buf, msg.c_str());
    write(sock_fd, buf, msg.size());
    memset(buf, 0, msg.size() + 1);
    free(buf);
}

void cc_socket::closeConnection()
{
    close(sock_fd);
}

void cc_socket::sendhendshack(int size)
{
    char num[10];
    sprintf(num, "%d", size);
    write(sock_fd, num, sizeof(num));
}

string cc_socket::receive()
{
    int size = receivehendshack();
    char *buf = (char *)malloc(sizeof(char) * (size + 1));
    memset(buf, 0, size + 1);
    read(sock_fd, buf, (size_t)size);
    string result = buf;
    memset(buf, 0, size + 1);
    free(buf);
    return result;
}

int cc_socket::receivehendshack()
{
    char num[10];
    read(sock_fd, num, sizeof(num));
    int size = atoi(num);
    return size;
}