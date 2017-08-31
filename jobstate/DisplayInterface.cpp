#include "DisplayInterface.hpp"
#include <iostream>

using namespace std;

DisplayInterface::DisplayInterface() {
    joblist = NULL;
    userName = "";
    noJob = true;
}

DisplayInterface::DisplayInterface(string list, string name) {
    joblist = json::parse(list);
    userName = name;
    noJob = true;
}

void DisplayInterface::noJobMessage() {
    if(noJob) {
        cout << "There is no job" << endl;
        separatedLine();
    }
}

void DisplayInterface::separatedLine() {
    cout << setw(60) << setfill('=') << "" << setfill(' ') << endl;
}