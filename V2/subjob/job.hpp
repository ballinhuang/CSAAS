#ifndef _JOB
#define _JOB

#include<iostream>
#include<json.hpp>
// for convenience
using json = nlohmann::json;

class job{

    public:
    void initjob();
    std::string encode_job();
    void addattirbute(std::string,std::string);
    json attribute;
};

#endif