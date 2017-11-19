#ifndef DISPLAYINTERFACE_HPP
#define DISPLAYINTERFACE_HPP

#include <json.hpp>
using json = nlohmann::json;
using namespace std;

class DisplayInterface {
    public:
        DisplayInterface();
        DisplayInterface(string, string, bool);
        virtual void displayState() = 0;
        virtual void displayAll() = 0;
        virtual void displayOneType(string) = 0;
        virtual void displayByID(int) = 0;
    protected:
        void noJobMessage();
        void separatedLine();

        bool noJob;
        bool root;
        json joblist;
        string userName;
};

#endif