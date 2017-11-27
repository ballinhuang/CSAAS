#include "s_nbsocket.hpp"
#include "Event.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{
    string ip, port;
    if (argc > 1)
    {
        ip = string(argv[1]);
        port = string(argv[2]);
    }
    else
    {
        cerr << "arg:[ip] [port]\n";
        return 0;
    }
    s_nbsocket s;
    s.setConnection(ip, port);
    while (true)
    {
        vector<Event> *events;
        events = s.getevent();
        for (vector<Event>::iterator it = events->begin(); it != events->end(); it++)
        {
            cout << "From fd : " << it->socket_fd << endl
                 << "Data : " << it->data << endl;
            s.sendmessage(it->socket_fd, it->data);
        }
    }
}