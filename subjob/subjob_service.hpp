#ifndef _SUBJOB_SERVICE
#define _SUBJOB_SERVICE

#include<iostream>
#include<json.hpp>
#include"Message.hpp"
class subjob_service{
    private:
    void setenv_attrubute(Message*);
    void parse_script(Message*,std::string);
    public:
    void creatjob(Message*,std::string);
};

#endif