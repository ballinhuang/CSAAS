#include "Display.hpp"
#include <iostream>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

Display::Display(string list) {
    joblist = json::parse(list);
}

void Display::displayState() {
    cout.setf(ios::left);
    cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        cout << setw(15) << "Job ID";
        cout << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name";
        cout << joblist["JOBNAME"][i].get<string>() << endl;
        
        cout << setw(15) << "Job State";
        cout << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User";
        cout << joblist["USER"][i].get<string>() << endl;

        cout << setw(15) << "MotherNode";
        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << joblist["MOTHERNODE"][i].get<string>() << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE")
            cout << joblist["RUNNODE"][i][0].get<string>() << "   (temporary)" << endl;

        cout << setw(15) << "Nodes";
        if(joblist["JOBSTAT"][i].get<string>() != "WAIT") {
            cout << setw(20) << joblist["RUNNODE"][i][0].get<string>() << " ---> ";
            cout.setf(ios::right);
            cout << setw(3) << joblist["RUNNP"][i][0].get<int>() << " CPUs" << endl;
            cout.unsetf(ios::right);
            for(int j = 1; j < (int)joblist["RUNNODE"][i].size(); j++) {
                cout << setw(15) << " ";
                cout << setw(20) << joblist["RUNNODE"][i][j].get<string>() << " ---> ";
                cout.setf(ios::right);
                cout << setw(3) << joblist["RUNNP"][i][j].get<int>() << " CPUs" << endl;
                cout.unsetf(ios::right);
            }
        }

        cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    }
    cout.unsetf(ios::left);
}