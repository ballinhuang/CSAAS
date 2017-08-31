#include "DetailDisplay.hpp"
#include <iostream>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

DetailDisplay::DetailDisplay(string list, string name) : DisplayInterface(list, name) {
}

void DetailDisplay::displayState() {
    cout.setf(ios::left);
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if(joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "FAIL" || joblist["USER"][i].get<string>() != userName)
            continue;

        noJob = false;

        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name" << joblist["JOBNAME"][i].get<string>().substr(0, 45) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>().substr(0, 45) << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>().substr(0, 45) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>().substr(0, 34) << "   (temporary)" << endl;

        if(joblist["JOBSTAT"][i].get<string>() != "WAIT") {
            cout << setw(15) << "Nodes" << setw(25) << joblist["RUNNODE"][i][0].get<string>().substr(0, 20) << " ---> ";
            cout.setf(ios::right);
            cout << setw(4) << joblist["RUNNP"][i][0].get<int>() << " CPUs" << endl;
            cout.unsetf(ios::right);
            for(int j = 1; j < (int)joblist["RUNNODE"][i].size(); j++) {
                cout << setw(15) << " ";
                cout << setw(25) << joblist["RUNNODE"][i][j].get<string>().substr(0, 20) << " ---> ";
                cout.setf(ios::right);
                cout << setw(4) << joblist["RUNNP"][i][j].get<int>() << " CPUs" << endl;
                cout.unsetf(ios::right);
            }
        }

        separatedLine();
    }
    cout.unsetf(ios::left);
    noJobMessage();
}

void DetailDisplay::displayOneType(string displayType) {
    cout.setf(ios::left);
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if((joblist["JOBSTAT"][i].get<string>() != displayType && !(displayType == "RUNNING" && joblist["JOBSTAT"][i].get<string>() == "READY")) || joblist["USER"][i].get<string>() != userName)
            continue;

        noJob = false;

        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name" << joblist["JOBNAME"][i].get<string>().substr(0, 45) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>().substr(0, 45) << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>().substr(0, 45) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>().substr(0, 34) << "   (temporary)" << endl;

            if(joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "READY") {
            cout << setw(15) << "Nodes" << setw(25) << joblist["RUNNODE"][i][0].get<string>().substr(0, 20) << " ---> ";
            cout.setf(ios::right);
            cout << setw(4) << joblist["RUNNP"][i][0].get<int>() << " CPUs" << endl;
            cout.unsetf(ios::right);
            for(int j = 1; j < (int)joblist["RUNNODE"][i].size(); j++) {
                cout << setw(15) << " ";
                cout << setw(25) << joblist["RUNNODE"][i][j].get<string>().substr(0, 20) << " ---> ";
                cout.setf(ios::right);
                cout << setw(4) << joblist["RUNNP"][i][j].get<int>() << " CPUs" << endl;
                cout.unsetf(ios::right);
            }
        }

        separatedLine();
    }
    cout.unsetf(ios::left);
    noJobMessage();
}

void DetailDisplay::displayDebug() {
    cout.setf(ios::left);
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if(joblist["USER"][i].get<string>() != userName)
            continue;

        noJob = false;

        cout << setw(15) << "Job ID" << joblist["JOBID"][i].get<int>() << endl;

        cout << setw(15) << "Job Name" << joblist["JOBNAME"][i].get<string>().substr(0, 45) << endl;
        
        cout << setw(15) << "Job State" << joblist["JOBSTAT"][i].get<string>() << endl;

        cout << setw(15) << "User" << joblist["USER"][i].get<string>().substr(0, 45) << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE")
            cout << setw(15) << "MotherNode" << joblist["MOTHERNODE"][i].get<string>().substr(0, 45) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << setw(15) << "MotherNode" << joblist["RUNNODE"][i][0].get<string>().substr(0, 34) << "   (temporary)" << endl;

        if(joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "READY") {
            cout << setw(15) << "Nodes" << setw(25) << joblist["RUNNODE"][i][0].get<string>().substr(0, 20) << " ---> ";
            cout.setf(ios::right);
            cout << setw(4) << joblist["RUNNP"][i][0].get<int>() << " CPUs" << endl;
            cout.unsetf(ios::right);
            for(int j = 1; j < (int)joblist["RUNNODE"][i].size(); j++) {
                cout << setw(15) << " ";
                cout << setw(25) << joblist["RUNNODE"][i][j].get<string>().substr(0, 20) << " ---> ";
                cout.setf(ios::right);
                cout << setw(4) << joblist["RUNNP"][i][j].get<int>() << " CPUs" << endl;
                cout.unsetf(ios::right);
            }
        }

        separatedLine();
    }
    cout.unsetf(ios::left);
    noJobMessage();
}