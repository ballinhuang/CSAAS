#ifndef _JOB_INFO_H
#define _JOB_INFO_H

#include "job_data.h"
#include <vector>
#include <map>
#include <string.h>
#include <iostream>
using namespace std;

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

  job_info();
  int add_attr(char *, char *, int );

  void parsedata(string *);

  Data_type getDataType();
  void setDataType(Data_type );
  
};

#endif