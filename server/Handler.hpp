#ifndef _HANDLER
#define _HANDLER

#include "IHandler.hpp"
#include <json.hpp>
#include "Monitor.hpp"
#include "s_socket.hpp"

using json = nlohmann::json;

class NewJobHandler : public IHandler
{
public:
  NewJobHandler(json, s_socket *);
  void handle();

private:
  json new_job;
  s_socket *s;
};

class RunJobHandler : public IHandler
{
public:
  RunJobHandler(json, s_socket *);
  void handle();

private:
  json req_runjob;
  s_socket *s;
};

class QueueStateHandler : public IHandler
{
public:
  QueueStateHandler(json, s_socket *);
  void handle();

private:
  json req_queue_state;
  s_socket *s;
};

class NodeStateHandler : public IHandler
{
public:
  NodeStateHandler(json, s_socket *);
  void handle();

private:
  json req_node_state;
  s_socket *s;
};

class DoneJobHandler : public IHandler
{
public:
  DoneJobHandler(json, s_socket *);
  void handle();

private:
  json req_done_job;
  s_socket *s;
};

class FailJobHandler : public IHandler
{
public:
  FailJobHandler(json, s_socket *);
  void handle();

private:
  json req_fail_job;
  s_socket *s;
};

class JobStateHandler : public IHandler
{
public:
  JobStateHandler(json, s_socket *);
  void handle();

private:
  json req_job_state;
  s_socket *s;
};

class RunStateHandler : public IHandler
{
public:
  RunStateHandler(json, s_socket *);
  void handle();

private:
  json req_runqueue_state;
  s_socket *s;
};

class KillJobHandler : public IHandler
{
public:
  KillJobHandler(json, s_socket *);
  void handle();

private:
  json req_kill_job;
  s_socket *s;
};

class ChangeModeHandler : public IHandler
{
public:
  ChangeModeHandler(json, s_socket *);
  void handle();

private:
  json req_change_mode;
  s_socket *s;
};

class GetTimeHandler : public IHandler
{
public:
  GetTimeHandler(json, s_socket *);
  void handle();

private:
  json req_get_time;
  s_socket *s;
};

class AddNodeHandler : public IHandler
{
public:
  AddNodeHandler(json, s_socket *);
  void handle();

private:
  json req_add_node;
  s_socket *s;
};

class RemoveNodeHandler : public IHandler
{
public:
  RemoveNodeHandler(json, s_socket *);
  void handle();

private:
  json req_remove_node;
  s_socket *s;
};
#endif