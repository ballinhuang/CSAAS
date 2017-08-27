#include "Display.hpp"
#include <iostream>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

Display::Display(string list) {
    joblist = json::parse(list);
    noJob = true;
}

void Display::displayState() {
    cout.setf(ios::left);
    cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if(joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "FAIL")
            continue;

        noJob = false;

        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name";
        if(joblist["JOBNAME"][i].get<string>().length() <= 35)
            cout << joblist["JOBNAME"][i].get<string>() << endl;
        else
            cout << joblist["JOBNAME"][i].get<string>().substr(0, 35) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>() << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>() << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>() << "   (temporary)" << endl;

        if(joblist["JOBSTAT"][i].get<string>() != "WAIT") {
            cout << setw(15) << "Nodes" << setw(20) << joblist["RUNNODE"][i][0].get<string>() << " ---> ";
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

    displayNoJobMessage();
}

void Display::displayOneType(string displayType) {
    cout.setf(ios::left);
    cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if(joblist["JOBSTAT"][i].get<string>() != displayType && !(displayType == "RUNNING" && joblist["JOBSTAT"][i].get<string>() == "READY"))
            continue;

        noJob = false;

        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name";
        if(joblist["JOBNAME"][i].get<string>().length() <= 35)
            cout << joblist["JOBNAME"][i].get<string>() << endl;
        else
            cout << joblist["JOBNAME"][i].get<string>().substr(0, 35) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>() << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>() << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>() << "   (temporary)" << endl;

        if(joblist["JOBSTAT"][i].get<string>() != "WAIT") {
            cout << setw(15) << "Nodes" << setw(20) << joblist["RUNNODE"][i][0].get<string>() << " ---> ";
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

    displayNoJobMessage();
}

void Display::displayDebug() {
    cout.setf(ios::left);
    cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name";
        if(joblist["JOBNAME"][i].get<string>().length() <= 35)
            cout << joblist["JOBNAME"][i].get<string>() << endl;
        else
            cout << joblist["JOBNAME"][i].get<string>().substr(0, 35) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>() << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>() << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>() << "   (temporary)" << endl;

        if(joblist["JOBSTAT"][i].get<string>() != "WAIT") {
            cout << setw(15) << "Nodes" << setw(20) << joblist["RUNNODE"][i][0].get<string>() << " ---> ";
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

void Display::displayNoJobMessage() {
    if(noJob) {
        cout << "There is no job" << endl;
        cout << setw(50) << setfill('=') << "" << setfill(' ') << endl;
    }
}