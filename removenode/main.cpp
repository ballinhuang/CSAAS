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

    string rmname = "";

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
            else if (rmname == "" && i == argc - 1)
            {
                rmname = string(argv[i]);
            }
            else
            {
                cout << "removenode ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }

    if ((ip == "" && port != "") || (ip != "" && port == ""))
    {
        cout << "removenode ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if (rmname == "")
    {
        cout << "removenode ---> main(): Error! node info wrong." << endl;
        exit(1);
    }

    cc_socket csock;
    if (csock.setConnection(ip, port) != 1)
    {
        cout << "removenode ---> main(): Error! setConnection error." << endl;
        exit(1);
    }
    if (csock.connect2server() != 1)
    {
        cout << "removenode ---> main(): Error! connect2server error." << endl;
        exit(1);
    }

    Message req_remove_node;

    req_remove_node.initMessage();
    req_remove_node.msg["NAME"] = rmname;
    req_remove_node.encode_Header("removenode", "server", "removenode");
    csock.send(req_remove_node.encode_Message());
    csock.closeConnection();

    return 0;
}