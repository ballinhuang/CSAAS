#ifndef _S_NBSOCKET
#define _S_NBSOCKET

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "Event.hpp"
#include <vector>

class s_nbsocket
{
public:
  int setConnection(std::string, std::string);
  int acceptClinet();
  std::vector<Event> *getevent();
  std::string readmessage(int fd);
  void closeConnection(int fd);
  void closebind();
  //void setacceptreuse();
  void sendmessage(int fd, std::string);

private:
  int receivehendshack(int fd);
  void sendhendshack(int fd, int size);
  int make_socket_non_blocking(int);
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  struct epoll_event event;
  int sock_fd, efd;
};

#endif