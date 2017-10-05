#ifndef _MOMHANDLER
#define _MOMHANDLER

#include "IHandler.hpp"
#include <json.hpp>
#include "s_socket.hpp"

using json = nlohmann::json;

class KillJobHandler : public IHandler
{
  public:
    KillJobHandler(json, s_socket *);
    void handle();

  private:
    json kill_job;
    s_socket *s;
};

#endif