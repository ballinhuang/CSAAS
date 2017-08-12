#include <iostream>
#include <fstream>
#include <string>

#include "Message.hpp"
#include "cc_socket.hpp"
#include "subjob_service.hpp"

using namespace std;

int main(int argc, char **argv)
{

    string script_name = "";
    string ip = "", port = "";
    if (argc > 1)
    {
        string arg;
        for (int i = 1; i < argc; i++)
        {
            arg = std::string(argv[i]);
            if (arg == "-i")
            {
                i++;
                ip = std::string(argv[i]);
            }
            else if (arg == "-p")
            {
                i++;
                port = std::string(argv[i]);
            }
            else if (script_name == "")
            {
                script_name = std::string(argv[i]);
            }
            else
            {
                cout << "Subjob ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }
    if (script_name == "")
    {
        cout << "Subjob ---> main(): Error! No input script file." << endl;
        exit(1);
    }
    if ((ip == "" && port != "") || (ip != "" && port == ""))
    {
        cout << "Subjob ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    Message newjob;
    newjob.initMessage();

    subjob_service srv;
    srv.creatjob(&newjob, script_name);

    if (ip == "" && port == "")
    {
        ifstream f;
        f.open("subjob.con");
        if (f.is_open())
        {
            f >> ip;
            f >> port;
        }
        else
        {
            cout << "Subjob ---> main(): Error! subjob.con not found." << endl;
            exit(1);
        }
    }

    cc_socket csock;
    if (csock.setConnection(ip, port) != 1)
    {
        exit(1);
    }
    if (csock.connect2server() != 1)
    {
        exit(1);
    }

    csock.send(newjob.encode_Message());
    string jobid;
    jobid = csock.receive();
    csock.closeConnection();

    cout << "The Job's ID is : " << jobid << endl;
}