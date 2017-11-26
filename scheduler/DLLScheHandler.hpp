#ifndef DLLSCHEHANDLER_HPP
#define DLLSCHEHANDLER_HPP

#include "IScheHandler.hpp"
#include "cc_socket.hpp"
#include "IScheduler.hpp"

class DLLScheHandler : public IScheHandler
{
public:
  DLLScheHandler(IScheduler *, cc_socket *);
  void handleschedule();

private:
  IScheduler *scheduler;
  cc_socket *socket;
};

#endif