#ifndef _JOB_INFO_H
#define _JOB_INFO_H

#include "job_data.h"
#include <vector>
#include <map>
#include <string.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <sstream>

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
  int jobid;
public:

  job_info();
  int add_attr(char *, char *, int );
  void setjobid(int id);
  void parsedata(string *);
  void readdata(char *message);
  void parse_job_tomom(string *);
  Data_type getDataType();
  void setDataType(Data_type );
  
};

#endif