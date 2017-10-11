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
    int jobid = NULL;

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
            else if (jobid == NULL && i == argc - 1)
            {
                jobid = atoi(argv[i]);
            }
            else
            {
                cout << "Killjob ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }

    if ((ip == "" && port != "") || (ip != "" && port == ""))
    {
        cout << "Killjob ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if (jobid == NULL)
    {
        cout << "Killjob ---> main(): Error! not select jobid." << endl;
        exit(1);
    }

    /*
    if (getuid() != 0)
    {
        cout << "Only root can killjob." << endl;
        exit(1);
    }
    */

    cc_socket csock;
    if (csock.setConnection(ip, port) != 1)
    {
        cout << "Killjob ---> main(): Error! setConnection error." << endl;
        exit(1);
    }
    if (csock.connect2server() != 1)
    {
        cout << "Killjob ---> main(): Error! connect2server error." << endl;
        exit(1);
    }

    Message req_killjob;

    req_killjob.initMessage();
    req_killjob.msg["JOBID"] = jobid;
    req_killjob.encode_Header("killjob", "server", "killjob");
    csock.send(req_killjob.encode_Message());
    csock.closeConnection();

    return 0;
}