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

    string adip = "", adport = "", adname = "";
    int adcore = 0;
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
            else if (adip == "" && i == argc - 4)
            {
                adip = string(argv[i]);
            }
            else if (adport == "" && i == argc - 3)
            {
                adport = string(argv[i]);
            }
            else if (adname == "" && i == argc - 2)
            {
                adname = string(argv[i]);
            }
            else if (adcore == 0 && i == argc - 1)
            {
                adcore = atoi(argv[i]);
            }
            else
            {
                cout << "addnode ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }

    if ((ip == "" && port != "") || (ip != "" && port == ""))
    {
        cout << "addnode ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if (adip == "" || adport == "" || adname == "" || adcore <= 0)
    {
        cout << "addnode ---> main(): Error! node info wrong." << endl;
        exit(1);
    }

    cc_socket csock;
    if (csock.setConnection(ip, port) != 1)
    {
        cout << "addnode ---> main(): Error! setConnection error." << endl;
        exit(1);
    }
    if (csock.connect2server() != 1)
    {
        cout << "addnode ---> main(): Error! connect2server error." << endl;
        exit(1);
    }

    Message req_add_node;

    req_add_node.initMessage();
    req_add_node.msg["IP"] = adip;
    req_add_node.msg["PORT"] = adport;
    req_add_node.msg["NAME"] = adname;
    req_add_node.msg["NP"] = adcore;
    req_add_node.encode_Header("addnode", "server", "addnode");
    csock.send(req_add_node.encode_Message());
    csock.closeConnection();

    return 0;
}