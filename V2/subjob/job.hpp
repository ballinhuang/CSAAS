#ifndef _JOB
#define _JOB

#include<iostream>
#include<json.hpp>
// for convenience
using json = nlohmann::json;

class job{

    public:
    void initjob();
    void setenv();
    void parse_script(std::string);
    std::string encode_job();
    private:
    json attribute;
};

#endif