#ifndef _EVENT
#define _EVENT

#include <iostream>

class Event
{
  public:
    int socket_fd;
    std::string data;
};

#endif