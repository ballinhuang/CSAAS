#ifndef _SCHEDULER_SERVICE
#define _SCHEDULER_SERVICE
#include "s_socket.hpp"
#include "s_nbsocket.hpp"
#include "Event.hpp"
#include "json.hpp"
#include "IScheduler.hpp"
#include "ScheHandlerFactory.hpp"
#include "IScheHandler.hpp"

#include <iostream>
using namespace std;
using json = nlohmann::json;

extern int debug;

class scheduler_service
{
public:
  scheduler_service(std::string, std::string, std::string);
  void do_schedule(std::string, std::string);
  void startservice();
  void set_mode(std::string);

private:
  void handlerequest(json);
  std::string schedule_mode, server_ip, server_port;
  ScheHandlerFactory *factory;
  IScheduler *scheduler;
  void *handle;
};

#endif