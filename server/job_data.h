#ifndef _JOB_DATA
#define _JOB_DATA
#include <iostream>
class job_data
  {
public:
    std::string name;
    std::string value;
    int var_type;
    //int op_type;
    //unsigned int flags;

    job_data(const char *pName,const char *pVal,int var):
      var_type(var)
      {
        if(pName == NULL) pName = "";
        if(pVal == NULL) pVal = "";
        name = pName;
        value = pVal;
      }
      std::string getname(){
        return name;
      }
      std::string getvalue(){
        return value;
      }
  };


  #endif