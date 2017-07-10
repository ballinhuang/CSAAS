#ifndef _MESSAGE
#define _MESSAGE

#include<iostream>
#include<json.hpp>
// for convenience
using json = nlohmann::json;

class Message{

    public:
    void initMessage();
    std::string encode_Message();
    void encode_Header(std::string,std::string,std::string);
    //void addattirbute(std::string,std::string);
    json msg;
};

#endif