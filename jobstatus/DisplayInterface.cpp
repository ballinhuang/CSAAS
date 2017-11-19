#include "DisplayInterface.hpp"
#include <iostream>

using namespace std;

DisplayInterface::DisplayInterface() {
    joblist = NULL;
    userName = "";
    noJob = true;
    root = false;
}

DisplayInterface::DisplayInterface(string list, string name, bool rootFlag) {
    joblist = json::parse(list);
    userName = name;
    noJob = true;
    root = rootFlag;
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