#ifndef _JOBSTRATER
#define _JOBSTRATER

#include <json.hpp>
#include <iostream>

using json = nlohmann::json;

class JobStrater
{
public:
  JobStrater(json, std::string, std::string);
  void start();

private:
  json req_run_job;
  std::string svr_ip, svr_port;
  std::string getsystemcall(const char *);
};

#endif