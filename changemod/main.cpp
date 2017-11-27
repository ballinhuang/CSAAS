#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>

#include "Message.hpp"
#include "cc_socket.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char **argv)
{
    string ip = "", port = "";
    string mode = "";

    if (argc > 1)
    {
        string arg;
        for (int i = 1; i < argc; i++)
        {
            arg = string(argv[i]);
            if (arg == "-i" && ip == "")
            {
                i++;
                ip = string(argv[i]);
            }
            else if (arg == "-p" && port == "")
            {
                i++;
                port = string(argv[i]);
            }
            else if (mode == "" && i == argc - 1)
            {
                mode = string(argv[i]);
            }
            else
            {
                cout << "changemode ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }

    if ((ip == "" && port != "") || (ip != "" && port == ""))
    {
        cout << "changemode ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if (mode == "")
    {
        cout << "changemode ---> main(): Error! not select mode." << endl;
        exit(1);
    }

    cc_socket csock;
    if (csock.setConnection(ip, port) != 1)
    {
        cout << "changemode ---> main(): Error! setConnection error." << endl;
        exit(1);
    }
    if (csock.connect2server() != 1)
    {
        cout << "changemode ---> main(): Error! connect2server error." << endl;
        exit(1);
    }

    if (mode == "default")
        mode = "";

    Message req_change_mode;

    req_change_mode.initMessage();
    req_change_mode.msg["MODE"] = mode;
    req_change_mode.encode_Header("changemod", "server", "change_mode");
    csock.send(req_change_mode.encode_Message());
    string state;
    state = csock.receive();
    csock.closeConnection();

    cout << state << endl;

    return 0;
}