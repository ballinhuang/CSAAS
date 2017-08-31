#ifndef DETAILDISPLAY_HPP
#define DETAILDISPLAY_HPP

#include "DisplayInterface.hpp"
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

class DetailDisplay : public DisplayInterface {
    public:
        DetailDisplay(string, string);
        void displayState();
        void displayOneType(string);
        void displayDebug();
};

#endif