#ifndef _SERVER
#define _SERVER

#include <iostream>
#include <ThreadPool.h>
#include <map>
#include <mutex>
#include <ctime>
#include "Observer.hpp"
#include "s_socket.hpp"
#include "HandlerFactory.hpp"

class Server : public Observer
{
public:
  Server();
  void set_server_attr(std::string, std::string);
  void set_scheduler_attr(std::string, std::string);
  void run();
  void notify(int);
  void attach_success();
  std::string getsvr_ip();
  std::string getsvr_port();

private:
  int contact_scheduler();
  void check_schedule();
  static void start_accept_thread(std::string, std::string, ThreadPool *);
  static void readrequest(s_socket *, HandlerFactory *);
  ThreadPool *request_pool;
  ThreadPool *server_pool;
  bool do_schedual = false;
  bool schedual_busy = false;
  int sched_iteration;
  time_t next_schedule, now_time;
  std::mutex do_schedual_tex, schedual_busy_tex;
  std::string svr_ip, svr_port, sch_ip, sch_port;
};

#endif