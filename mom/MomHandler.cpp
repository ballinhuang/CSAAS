#include "MomHandler.hpp"
#include <json.hpp>
#include "Message.hpp"
#include "JobQueue.hpp"
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

using json = nlohmann::json;
using namespace std;

extern int debug;
extern ofstream *debug_file;

//NewJobHandler start

RunJobHandler::RunJobHandler(json job, s_socket *socket)
{
    req_run_job = job;
    s = socket;
}

void RunJobHandler::handle()
{
    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> RunJobHandler handle(): Start check command path." << endl;
        else if (debug == 2)
            cout << "MOM ---> RunJobHandler handle(): Start check command path." << endl;
    }
    int command_count = (int)req_run_job["SCRIPT"].size();
    bool can_run = true;
    for (int i = 0; i < command_count; i++)
    {
        stringstream ss;
        ss << req_run_job["SCRIPT"][i].get<string>();
        string command;
        ss >> command;
        // ./XXX.exe
        if (command.at(0) == '.' && command.at(1) == '/')
        {
            //default let it legal
            continue;
        }
        char *env_path;
        env_path = (char *)malloc(sizeof(char) * req_run_job["ENV"]["PATH"].get<string>().length());
        strcpy(env_path, req_run_job["ENV"]["PATH"].get<string>().c_str());
        char *path = strtok(env_path, ":");
        bool can_exe = false;
        while (path != NULL)
        {
            string check_path = path;
            check_path = check_path + "/" + command;
            if (access(check_path.c_str(), X_OK) == 0)
            {
                req_run_job["SCRIPTPATH"][i] = check_path;
                can_exe = true;
                free(env_path);
                break;
            }
            path = strtok(NULL, ":");
        }
        if (!can_exe)
        {
            can_run = false;
            free(env_path);
            break;
        }
    }

    if (!can_run)
    {
        // reject job
        return;
    }

    if (debug > 0)
    {
        if (debug == 1)
            *debug_file << "MOM ---> RunJobHandler handle(): Command can run." << endl;
        else if (debug == 2)
            cout << "MOM ---> RunJobHandler handle(): Command can run." << endl;
    }

    /*UID GID*/
    struct passwd *user;
    user = getpwnam(req_run_job["ENV"]["USER"].get<string>().c_str());
    uid_t uid = user->pw_uid;
    gid_t gid = user->pw_gid;

    /*OUTPUT FILE*/
    char rootdir[1024];
    getcwd(rootdir, sizeof(rootdir));
    char fileName[100];
    sprintf(fileName, "JOB%d.OUT", req_run_job["JOBID"].get<int>());
    int fd_out = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /*env*/
    char *envp[10] = {0};
    envp[0] = (char *)malloc(sizeof(char) * req_run_job["ENV"]["PATH"].get<string>().length() + 6);
    strcpy(envp[0], "PATH=");
    strcat(envp[0], req_run_job["ENV"]["PATH"].get<string>().c_str());

    for (int i = 0; i < command_count; i++)
    {
        // nodefile creat
        bool is_parallel = false;
        if (req_run_job["SCRIPT"][i].get<string>().find("<nodefile>") != -1)
        {
            ofstream nodefile;
            nodefile.open("nodefile");
            for (int n = 0; n < req_run_job["RUNNODE"].size(); n++)
            {
                nodefile << req_run_job["RUNNODE"][n].get<string>();
                nodefile << " slots=" << req_run_job["RUNNP"][n].get<int>();
                nodefile << endl;
            }
            nodefile.close();
            is_parallel = true;
            // change file owner
            chown("nodefile", uid, gid);

            //  move file to home
            char node_old_filepath[1000] = "";
            strcat(node_old_filepath, rootdir);
            strcat(node_old_filepath, "/");
            strcat(node_old_filepath, "nodefile");

            char node_new_filepath[1000] = "";
            strcat(node_new_filepath, req_run_job["ENV"]["HOME"].get<string>().c_str());
            strcat(node_new_filepath, "/");
            strcat(node_new_filepath, "nodefile");

            rename(node_old_filepath, node_new_filepath);
        }

        pid_t p;
        int status;
        p = fork();
        if (p < 0)
        {
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> RunJobHandler handle(): fork() fail!" << endl;
                else if (debug == 2)
                    cout << "MOM ---> RunJobHandler handle(): fork() fail!" << endl;
            }
            exit(1);
        }
        else if (p == 0)
        {
            /*Change user*/
            setuid(uid);
            /*change dir*/
            chdir(req_run_job["ENV"]["HOME"].get<string>().c_str());
            /* If nodefile need creat it.*/
            // is check ./xxx.exe
            char cwd[1024];
            if (req_run_job["SCRIPT"][i].get<string>().at(0) == '.' && req_run_job["SCRIPT"][i].get<string>().at(1) == '/')
            {
                getcwd(cwd, sizeof(cwd));
                string cwd_str(cwd);
                //remove "./"
                string temp = req_run_job["SCRIPT"][i].get<string>();
                temp.erase(0, 2);
                req_run_job["SCRIPT"][i] = temp;
                req_run_job["SCRIPTPATH"][i] = cwd_str + "/" + req_run_job["SCRIPT"][i].get<string>().substr(0, req_run_job["SCRIPT"][i].get<string>().find(" "));
            }

            // argv parse
            char *argv[10] = {0};
            stringstream ss;
            ss << req_run_job["SCRIPT"][i].get<string>();
            string argvstr;
            for (int i = 0; ss >> argvstr && i < 10; i++)
            {
                if (argvstr.compare("<nodefile>") == 0)
                {
                    argv[i] = (char *)malloc(sizeof(char) * (argvstr.size() - 2));
                    strcpy(argv[i], "nodefile");
                }
                else
                {
                    argv[i] = (char *)malloc(sizeof(char) * argvstr.size());
                    strcpy(argv[i], argvstr.c_str());
                }
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);

            execve(req_run_job["SCRIPTPATH"][i].get<string>().c_str(), argv, envp);

            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> RunJobHandler handle(): execve() fail!" << endl;
                else if (debug == 2)
                    cerr << "MOM ---> RunJobHandler handle(): execve() fail!" << endl;
            }
            exit(1);
        }
        else
        {
            if (debug > 0)
            {
                if (debug == 1)
                    *debug_file << "MOM ---> RunJobHandler handle(): execve() the command = " << req_run_job["SCRIPT"][i].get<string>() << endl;
                else if (debug == 2)
                    cout << "MOM ---> RunJobHandler handle(): execve() the command = " << req_run_job["SCRIPT"][i].get<string>() << endl;
            }
            //parent waite job complete
            waitpid(p, &status, 0);

            // remove the nodefile
            if (is_parallel)
            {
                char node_new_filepath[1000] = "";
                strcat(node_new_filepath, req_run_job["ENV"]["HOME"].get<string>().c_str());
                strcat(node_new_filepath, "/");
                strcat(node_new_filepath, "nodefile");
                remove(node_new_filepath);
            }
        }
    }

    // change file owner
    chown(fileName, uid, gid);
    //  move file to home
    char output_old_path[1000] = "";
    strcat(output_old_path, rootdir);
    strcat(output_old_path, "/");
    strcat(output_old_path, fileName);
    char output_new_path[1000] = "";
    strcat(output_new_path, req_run_job["ENV"]["HOME"].get<string>().c_str());
    strcat(output_new_path, "/");
    strcat(output_new_path, fileName);
    rename(output_old_path, output_new_path);

    req_run_job.erase("SCRIPTPATH");
    req_run_job["PID"] = 0;
    JobQueue::GetInstance()->addjob(req_run_job["JOBID"].get<int>(), req_run_job);
    JobQueue::GetInstance()->notitfyjobdone();
}

//NewJobHandler end
