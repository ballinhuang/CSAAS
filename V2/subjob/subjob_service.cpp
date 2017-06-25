#include"subjob_service.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>

using namespace std;
// for convenience
using json = nlohmann::json;

void subjob_service::setenv_attrubute(job *j){
    string env_need[]={"HOME","HOSTNAME","PATH","TZ","USER","MAIL","SHELL","LANG"};
	char *envdata;
	for(int i=0 ; i < sizeof(env_need)/sizeof(env_need[0]) ; i++)
	{
		if( envdata = getenv(env_need[i].c_str()) ){
			j->attribute["ENV"][env_need[i]] = envdata;
		}
	}
}

void subjob_service::parse_script(job *j,string script_name){
    struct stat statbuf;
    if (stat(script_name.c_str(), &statbuf) < 0){
        std::cout << "err: script file cannot be loaded\n";
        exit(1);
    }

    if (!S_ISREG(statbuf.st_mode)){
        cout <<"err: script not a file\n";
        exit(1);
    }

    ifstream f(script_name);
    string line;
    int count = 0;
    while(getline(f,line)){
        j->attribute["SCRIPT"][count] = line;
        count++;
    }
}

void subjob_service::creatjob(job *j,string script_name){
    setenv_attrubute(j);
    parse_script(j,script_name);
}
