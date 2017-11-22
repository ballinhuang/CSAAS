#include "scheduler_service.hpp"
#include "cc_socket.hpp"
#include "ScheHandlerFactory.hpp"
#include "IScheHandler.hpp"
using namespace std;

scheduler_service::scheduler_service(string mode, string ip, string port)
{
    schedule_mode = mode;
    server_ip = ip;
    server_port = port;
}

int scheduler_service::do_schedule()
{
    cc_socket *socket = new cc_socket();

    if (socket->setConnection(server_ip, server_port) == 0)
    {
        return 0;
    }

    if (socket->connect2server() == 0)
    {
        return 0;
    }

    ScheHandlerFactory factory;
    IScheHandler *ScheHandler = factory.getScheHandler(schedule_mode, socket);

    ScheHandler->handleschedule();
    socket->closeConnection();
    return 1;
}