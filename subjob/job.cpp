#include"job.hpp"
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

void job::initjob(){
    attribute.clear();
}

void job::setenv(){
    string env_need[]={"HOME","HOSTNAME","PATH","TZ","USER","MAIL","SHELL","LANG"};
	char *envdata;
	for(int i=0 ; i < sizeof(env_need)/sizeof(env_need[0]) ; i++)
	{
		if( envdata = getenv(env_need[i].c_str()) ){
			attribute[env_need[i]] = envdata;
		}
	}
}

void job::parse_script(string script_name){
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
        attribute["SCRIPT"][count] = line;
        count++;
    }
}

string job::encode_job(){
    string result = attribute.dump();
    return result;
}