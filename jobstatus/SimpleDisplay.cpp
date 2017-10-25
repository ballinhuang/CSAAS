#include "SimpleDisplay.hpp"
#include <iostream>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

SimpleDisplay::SimpleDisplay(string list, string name, bool root) : DisplayInterface(list, name, root) {
}

void SimpleDisplay::displayState() {
    cout.setf(ios::left);
    cout << setw(8) << "ID" << setw(17) << "Name" << setw(11) << "State" << setw(10) << "User" << "MotherNode" << endl;
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if((joblist["USER"][i].get<string>() != userName && !root) || joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "FAIL" || joblist["JOBSTAT"][i].get<string>() == "RUNFAIL")
            continue;

        noJob = false;

        cout << setw(8) << joblist["JOBID"][i].get<int>();
        cout << setw(17) << joblist["JOBNAME"][i].get<string>().substr(0, 15);
        cout << setw(11) << joblist["JOBSTAT"][i].get<string>();
        cout << setw(10) << joblist["USER"][i].get<string>().substr(0, 9);
        
        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING")
            cout << joblist["MOTHERNODE"][i].get<string>().substr(0, 12) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << joblist["RUNNODE"][i][0].get<string>().substr(0, 8) << "(temp)" << endl;
        else
            cout << endl;
    }
    if(!noJob)
        separatedLine();
    cout.unsetf(ios::left);
    noJobMessage();
}

void SimpleDisplay::displayOneType(string displayType) {
    cout.setf(ios::left);
    cout << setw(8) << "ID" << setw(17) << "Name" << setw(11) << "State" << setw(10) << "User" << "MotherNode" << endl;
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if((joblist["USER"][i].get<string>() != userName && !root) || (joblist["JOBSTAT"][i].get<string>() != displayType && !(displayType == "RUNNING" && joblist["JOBSTAT"][i].get<string>() == "READY")))
            continue;

        noJob = false;

        cout << setw(8) << joblist["JOBID"][i].get<int>();
        cout << setw(17) << joblist["JOBNAME"][i].get<string>().substr(0, 15);
        cout << setw(11) << joblist["JOBSTAT"][i].get<string>();
        cout << setw(10) << joblist["USER"][i].get<string>().substr(0, 9);
        
        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "RUNFAIL")
            cout << joblist["MOTHERNODE"][i].get<string>().substr(0, 12) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << joblist["RUNNODE"][i][0].get<string>().substr(0, 8) << "(temp)" << endl;
        else
            cout << endl;
    }
    if(!noJob)
        separatedLine();
    cout.unsetf(ios::left);
    noJobMessage();
}

void SimpleDisplay::displayAll() {
    cout.setf(ios::left);
    cout << setw(8) << "ID" << setw(17) << "Name" << setw(11) << "State" << setw(10) << "User" << "MotherNode" << endl;
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++) {
        if(joblist["USER"][i].get<string>() != userName && !root)
            continue;

        noJob = false;

        cout << setw(8) << joblist["JOBID"][i].get<int>();
        cout << setw(17) << joblist["JOBNAME"][i].get<string>().substr(0, 15);
        cout << setw(11) << joblist["JOBSTAT"][i].get<string>();
        cout << setw(10) << joblist["USER"][i].get<string>().substr(0, 9);
        
        if(joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "RUNFAIL")
            cout << joblist["MOTHERNODE"][i].get<string>().substr(0, 12) << endl;
        else if(joblist["JOBSTAT"][i].get<string>() == "READY")
            cout << joblist["RUNNODE"][i][0].get<string>().substr(0, 8) << "(temp)" << endl;
        else
            cout << endl;
    }
    if(!noJob)
        separatedLine();
    cout.unsetf(ios::left);
    noJobMessage();
}

void SimpleDisplay::displayByID(int ID) {
    cout.setf(ios::left);
    cout << setw(8) << "ID" << setw(17) << "Name" << setw(11) << "State" << setw(10) << "User" << "MotherNode" << endl;
    separatedLine();
    for(int i = 0; i < (int)joblist["JOBID"].size(); i++)
        if((joblist["USER"][i].get<string>() == userName || root) && joblist["JOBID"][i].get<int>() == ID) {
            noJob = false;

            cout << setw(8) << joblist["JOBID"][i].get<int>();
            cout << setw(17) << joblist["JOBNAME"][i].get<string>().substr(0, 15);
            cout << setw(11) << joblist["JOBSTAT"][i].get<string>();
            cout << setw(10) << joblist["USER"][i].get<string>().substr(0, 9);
            
            if(joblist["JOBSTAT"][i].get<string>() == "RUNNING" || joblist["JOBSTAT"][i].get<string>() == "COMPLETE" || joblist["JOBSTAT"][i].get<string>() == "RUNFAIL")
                cout << joblist["MOTHERNODE"][i].get<string>().substr(0, 12) << endl;
            else if(joblist["JOBSTAT"][i].get<string>() == "READY")
                cout << joblist["RUNNODE"][i][0].get<string>().substr(0, 8) << "(temp)" << endl;
            else
                cout << endl;

            separatedLine();
            break;
        }
    cout.unsetf(ios::left);
    if(noJob) {
        cout << "This ID does not exist or this job is not yours" << endl;
        separatedLine();
    }
}