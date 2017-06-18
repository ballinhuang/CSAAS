#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include "qsub_function.h"
using namespace std;

int read_env_data(job_info *ji)
{
	char env_need[][10]={"HOME","HOSTNAME","PATH","TZ","USER","MAIL","SHELL","LANG"};
	char *envdata;
	for(int i=0 ; i < sizeof(env_need)/sizeof(env_need[0]) ; i++)
	{
		if( envdata = getenv(env_need[i]) ){
			ji->add_attr(env_need[i], envdata, ENV_DATA);
		}
	}
}

int read_script_data(job_info *ji , char *script_name)
{
	FILE *ptr;
	char buf[1000] = {0};
	ptr = fopen(script_name, "r");
	char script_type[100],script_parm[1000]="";
	if(ptr == NULL) {
		cout << "IPConfig doesn't exit !" << endl;
		return 0;
	}
	strcpy(script_type,"Script");
	while(fgets(buf, sizeof(buf), ptr)) {
		strcat(script_parm,buf);
		strcat(script_parm,"\n");
	}
	strcat(script_parm,"Script\n");
	ji->add_attr(script_type, script_parm, SCRIPT_DATA);
	return 1;
}

string get_job_info(int argc, char **argv,char **envp)
{
	job_info job= job_info();
	job.setDataType(QSUB);
	char *script_name;
	int debug;
	
	if (argc > 0 && argv[1] != NULL)
	{
		script_name = argv[1];
	}
	else
		return NULL;
	read_env_data(&job);
	read_script_data(&job,script_name);

	string parse;
	job.parsedata(&parse);

	return parse;
}
