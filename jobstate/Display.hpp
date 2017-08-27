#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <json.hpp>
using json = nlohmann::json;

class Display {
    public:
        Display(std::string list);
        void displayState();
        void displayDebug();
        void displayOneType(std::string);
        void displayNoJobMessage();
    private:
        json joblist;
        bool noJob;
};

#endif