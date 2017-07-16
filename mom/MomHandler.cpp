#include "MomHandler.hpp"
#include <json.hpp>
#include "Message.hpp"
#include "JobQueue.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <signal.h>
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

void RunJobHandler::sig_fork(int signo)
{
    int stat;
    waitpid(0,&stat,WNOHANG);
    JobQueue::GetInstance()->notitfyjobdone();
    return;
}

void RunJobHandler::handle(){
    int command_count = (int)req_run_job["SCRIPT"].size();
    char *env_path;
    env_path = (char*)malloc(sizeof(char) * req_run_job["ENV"]["PATH"].get<string>().length());
    bool can_run = true;
    for(int i = 0 ; i < command_count ; i++){
        stringstream ss;
        ss << req_run_job["SCRIPT"][i].get<string>();
        string command;
        ss >> command;
        char *path = strtok(env_path,":");
        bool can_exe = false;
        while(path != NULL){
            string check_path = path;
            check_path = check_path + "/" + command;
            if( access(check_path.c_str(), X_OK) == 0 ){
                req_run_job["SCRIPTPATH"][i] = check_path;
                can_exe = true;
                break;
            }
            path = strtok(NULL,":");
        }
        if(!can_exe){
            can_run = false;
            break;
        }
    }

    if(!can_run){
        // reject job
        return;
    }

    signal (SIGCHLD, sig_fork);
    int status;
    pid_t pid;

    pid = fork();
    if (pid < 0){
        // job fail
        return;
    }
    else if (pid == 0)
    {
        char *envp[10]={0};
        envp[0] = (char*)malloc(sizeof(char) * req_run_job["ENV"]["PATH"].get<string>().length()+6);
        strcpy(envp[0],"PATH=");
        strcat(envp[0],req_run_job["ENV"]["PATH"].get<string>().c_str());
        char fileName[100];
        sprintf(fileName,"JOB%d.OUT", req_run_job["JOBID"].get<int>());
        int fd_out = open(fileName, O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
        for(int i = 0 ; i < command_count ; i++){
            pid_t p;
            p = fork();
            if (p < 0){
                // job fail
                exit(1);
            }
            else if(p == 0){
                char *argv[10]={0};
                stringstream ss;
                ss << req_run_job["SCRIPT"][i].get<string>();
                string argvstr;
                for (int i = 0; ss >> argvstr && i < 10 ; i++){
                    argv[i] = (char*)malloc(sizeof(char) * argvstr.size());
                    strcpy(argv[i],argvstr.c_str()); 		
                }
                execve(req_run_job["SCRIPTPATH"][i].get<string>().c_str(),argv,envp);
            }
            else{
                waitpid( pid, &status, 0 );
                //parent waite job complete
            }
        }
        exit(1);
    }
    req_run_job.erase("SCRIPTPATH");
    req_run_job["PID"] = pid;
    JobQueue::GetInstance()->addjob(req_run_job["JOBID"].get<int>(),req_run_job);
}

//NewJobHandler end
