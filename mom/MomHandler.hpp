#ifndef _MOMHANDLER
#define _MOMHANDLER

#include "IHandler.hpp"
#include <json.hpp>
#include "s_socket.hpp"

using json = nlohmann::json;

class RunJobHandler : public IHandler
{
  public:
    RunJobHandler(json, s_socket *);
    void handle();

  private:
    json req_run_job;
    s_socket *s;
};

#endif