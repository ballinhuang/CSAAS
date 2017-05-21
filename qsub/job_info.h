#ifndef _JOB_INFO_H
#define _JOB_INFO_H

#include "job_data.h"
#include <vector>
#include <map>
#include <string.h>
using namespace std;
/*var_type*/
#define ENV_DATA      1 
#define CMDLINE_DATA  2 
#define LOGIC_DATA    3 
#define STATIC_DATA   4 
#define CONFIG_DATA   5 
#define FILTER_DATA   6
#define SCRIPT_DATA   7

enum Data_type {QSUB, SCHEDULER};

class job_info
{
private:
  map<string,job_data> job_attr;
  Data_type data_type;

public:

  job_info()
  {
      job_attr = map<string,job_data>();
  }
  int add_attr(char *name, char *value, int type)
  {
    job_data data = job_data(name,value,type);
    job_attr.insert(pair<string,job_data>(name,data));
    return 1;
  }
  void parsedata(string *parse){
    *parse += "TYPE = ";
    switch(data_type) {
      case QSUB:  *parse += "QSUB"; break;
      case SCHEDULER:  *parse += "SCHEDULER"; break;
      default:  break;
    }
    *parse += "\n";

    for (map<string,job_data>::iterator it = job_attr.begin(); it != job_attr.end(); it++)
       *parse = *parse + it->second.name + " = " + it->second.value + "\n";
  }
  Data_type getDataType() {
    return data_type;
  }
  void setDataType(Data_type type) {
    data_type = type;
  }
};

#endif