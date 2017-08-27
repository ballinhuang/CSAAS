#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "Message.hpp"
#include "cc_socket.hpp"
#include "Display.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char **argv) {
    string ip = "", port = "", displayType = "";
    if(argc > 1) {
        string arg;
        for(int i = 1; i < argc; i++) {
            arg = string(argv[i]);
            if(arg == "-i") {
                i++;
                ip = string(argv[i]);
            }
            else if(arg == "-p") {
                i++;
                port = string(argv[i]);
            }
            else if(arg == "-t") {
                i++;
                displayType = string(argv[i]);
                for(int i = 0; i < (int)displayType.size(); i++)
                    if(displayType[i] >= 'a' && displayType[i] <= 'z')
                        displayType[i] = displayType[i] - 'a' + 'A';

                if(displayType != "DEBUG" && displayType != "COMPLETE" && displayType != "RUNNING" && displayType != "WAIT" && displayType != "FAIL") {
                    cout << "JobState ---> main(): Error state " << endl;
                    exit(1);
                }
            }
            else {
                cout << "JobState ---> main(): Error parameter " << arg << endl;
                exit(1);
            }
        }
    }
    
    if((ip == "" && port != "") || (ip != "" && port == "")) {
        cout << "JobState ---> main(): Error! -i -p must be used at the same time." << endl;
        exit(1);
    }

    if(ip == "" && port == "") {
        ifstream f;
        f.open("../subjob/subjob.con");
        if(f.is_open()) {
            f >> ip;
            f >> port;
        }
        else {
            cout << "JobState ---> main(): Error! subjob.con can't found." << endl;
            exit(1);
        }
    }

    cc_socket csock;
    if(csock.setConnection(ip, port) != 1) {
        exit(1);
    }
    if(csock.connect2server() != 1) {
        exit(1);
    }

    Message req_state;
    string list;

    req_state.initMessage();
    req_state.encode_Header("jobstate", "server", "jobliststate");
    csock.send(req_state.encode_Message());
    list = csock.receive();
    csock.closeConnection();

    Display display(list);
    if(displayType == "")
        display.displayState();
    else if(displayType == "DEBUG")
        display.displayDebug();
    else
        display.displayOneType(displayType);

    return 0;
}