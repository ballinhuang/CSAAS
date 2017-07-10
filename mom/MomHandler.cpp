#include "MomHandler.hpp"
#include <json.hpp>
#include "Message.hpp"
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <cstring>

using json = nlohmann::json;
using namespace std;

extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

RunJobHandler::RunJobHandler(json job, s_socket *socket){
    req_run_job = job;
    s = socket;
}

void RunJobHandler::handle(){

    string task = req_run_job["SCRIPT"][0].get<string>();
    stringstream ss;
    string argvstr,file;
    char *argv[10]={0};
    ss << task;
    ss >> file;
    argv[0] = (char*)malloc(sizeof(char) * file.size());
    strcpy(argv[0],file.c_str());
    int i = 1;
    for (; ss >> argvstr && i<10 ; i++){
    	argv[i] = (char*)malloc(sizeof(char) * argvstr.size());
    	strcpy(argv[i],argvstr.c_str()); 		
    }

    int pid;
    int pfd[2];

    if(pipe(pfd)<0)
    {
        if(debug > 0){
            if(debug == 1)
                *debug_file << "RunJobHandler handle(): Pipe ERROR!" << endl;
            else if(debug == 2)
                cout << "RunJobHandler handle(): Pipe ERROR!" << endl;
        }
    	return;
    }

    pid = fork();

    if(pid == -1) {
        if(debug > 0){
            if(debug == 1)
                *debug_file << "RunJobHandler handle(): Fork ERROR!" << endl;
            else if(debug == 2)
                cout << "RunJobHandler handle(): Fork ERROR!" << endl;
        }
		return;
	}
    else if(pid == 0){
		dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);

        /* execute CGI */
        execvp( file.c_str(), argv );
        exit(0);
	}
    else{ 
        /* parent process */
        close(pfd[1]);
        char fileName[100];
    	sprintf(fileName,"JOB%d.OUT", req_run_job["JOBID"].get<int>());
    	FILE *file_ptr = fopen(fileName, "w");
        char buffer[1024] = "";
        int len;
        memset(buffer , 0 , sizeof(buffer));
        while( (len = read(pfd[0], buffer, 1023 )) > 0) {
    		fprintf(file_ptr, "%s", buffer);
            memset(buffer , 0 , sizeof(buffer));
        }
        fclose(file_ptr);
    }
}

//NewJobHandler end
