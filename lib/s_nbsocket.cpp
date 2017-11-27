#include "s_nbsocket.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <cstring>
using namespace std;

int s_nbsocket::setConnection(string ip, string port)
{
    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(stoi(port));
    server_addr.sin_family = AF_INET;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return 0;
    }

    int on = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        close(sock_fd);
        return 0;
    }
    /* Set socket to be nonblocking.*/
    if (ioctl(sock_fd, FIONBIO, (char *)&on) < 0)
    {
        close(sock_fd);
        return 0;
    }
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        close(sock_fd);
        return 0;
    }

    if (listen(sock_fd, 32) == -1)
    {
        close(sock_fd);
        return 0;
    }

    if ((efd = epoll_create1(0)) == -1)
    {
        close(sock_fd);
        return 0;
    }
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event) == -1)
    {
        close(sock_fd);
        return 0;
    }

    return 1;
}

int s_nbsocket::acceptClinet()
{
    while (true)
    {
        memset(&client_addr, '\0', sizeof(struct sockaddr_in));
        int sin_size = sizeof(struct sockaddr_in);
        int fd;
        fd = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size);
        if (fd == -1)
        {
            break;
        }
        if (make_socket_non_blocking(fd) == -1)
        {
            closeConnection(fd);
            continue;
        }
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            close(fd);
            return 0;
        }
    }
    return 1;
}

vector<Event> *s_nbsocket::getevent()
{
    vector<Event> *ioevents = new vector<Event>();
    struct epoll_event *events;
    events = (epoll_event *)calloc(128, sizeof event);
    int io_ready;
    io_ready = epoll_wait(efd, events, 128, -1);
    for (int i = 0; i < io_ready; i++)
    {
        if ((events[i].events & EPOLLERR) ||
            (events[i].events & EPOLLHUP) ||
            (!(events[i].events & EPOLLIN)))
        {
            close(events[i].data.fd);
            continue;
        }
        else if (events[i].data.fd == sock_fd)
        {
            if (acceptClinet() == 0)
            {
                return NULL;
            }
        }
        else
        {
            Event ev;
            ev.socket_fd = events[i].data.fd;
            string msg = readmessage(events[i].data.fd);
            if (msg != "")
            {
                ev.data = msg;
                ioevents->push_back(ev);
            }
        }
    }
    return ioevents;
}

string s_nbsocket::readmessage(int fd)
{
    string result = "";
    int size;
    do
    {
        size = receivehendshack(fd);
        if (size == 0)
        {
            break;
        }
        else if (size == -1)
        {
            closeConnection(fd);
            break;
        }
        char *buf = (char *)malloc(sizeof(char) * (size + 1));
        memset(buf, 0, size + 1);
        while (read(fd, buf, (size_t)size) != (size_t)size)
            ;
        result = buf;
        memset(buf, 0, size + 1);
        free(buf);
    } while (true);
    return result;
}

int s_nbsocket::receivehendshack(int fd)
{
    char num[10];
    int rc;
    rc = recv(fd, num, sizeof(num), 0);
    if (rc == 0)
    {
        //close
        return -1;
    }
    else if (rc < 0)
    {
        return 0;
    }
    int size = atoi(num);
    return size;
}
void s_nbsocket::sendmessage(int fd, string msg)
{
    sendhendshack(fd, msg.size());
    char *buf = (char *)malloc(sizeof(char) * (msg.size() + 1));
    memset(buf, 0, msg.size() + 1);
    strcpy(buf, msg.c_str());
    write(fd, buf, msg.size());
    memset(buf, 0, msg.size() + 1);
    free(buf);
}

void s_nbsocket::sendhendshack(int fd, int size)
{
    char num[10];
    sprintf(num, "%d", size);
    write(fd, num, sizeof(num));
}

void s_nbsocket::closebind()
{
    close(sock_fd);
}

void s_nbsocket::closeConnection(int fd)
{
    close(fd);
}
/*
void s_socket::setacceptreuse()
{
    int sockoptval = 1;
    setsockopt(conn_port, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval));
}
*/

int s_nbsocket::make_socket_non_blocking(int sfd)
{
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1)
    {
        perror("fcntl");
        return -1;
    }

    return 0;
}