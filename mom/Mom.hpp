#ifndef _MOM
#define _MOM

#include <iostream>
#include <ThreadPool.h>
#include <map>
#include <mutex>
#include "Observer.hpp"
#include "s_socket.hpp"
#include "MomHandlerFactory.hpp"
#include <json.hpp>

using json = nlohmann::json;

class Mom : public Observer
{
  public:
    void set_mom_attr(std::string, std::string);
    void set_server_attr(std::string, std::string);
    void run();
    void notify(int);
    void attach_success();

  private:
    static void sig_fork(int);
    static void readrequest(s_socket *, MomHandlerFactory *, json);
    ThreadPool *request_pool;
    std::string svr_ip, svr_port, mom_ip, mom_port;
};

#endif