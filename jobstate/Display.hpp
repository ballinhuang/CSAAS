#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <json.hpp>
using json = nlohmann::json;

class Display {
    public:
        Display(std::string list);
        void displayState();
    private:
        json joblist;
};

#endif