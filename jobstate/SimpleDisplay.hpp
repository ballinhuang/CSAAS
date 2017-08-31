#ifndef SIMPLEDISPLAY_HPP
#define SIMPLEDISPLAY_HPP

#include "DisplayInterface.hpp"
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

class SimpleDisplay : public DisplayInterface {
    public:
        SimpleDisplay(string, string);
        void displayState();
        void displayOneType(string);
        void displayDebug();
        void displayByID(int);
};

#endif