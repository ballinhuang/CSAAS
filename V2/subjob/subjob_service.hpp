#ifndef _SUBJOB_SERVICE
#define _SUBJOB_SERVICE

#include<iostream>
#include<json.hpp>
#include"job.hpp"
class subjob_service{
    public:
    void setenv_attrubute(job*);
    void parse_script(job*,std::string);
    void creatjob(job*,std::string);
};

#endif