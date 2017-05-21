#ifndef _QSUB_FINCTION_H
#define _QSUB_FINCTION_H
using namespace std;
#include "job_info.h"
int read_env_data(job_info *ji);
int read_script_data(job_info *ji , char *script_name);
string get_job_info(int argc, char **argv,char **envp);

#endif