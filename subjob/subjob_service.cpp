#include "subjob_service.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
// for convenience
using json = nlohmann::json;

string subjob_service::trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void subjob_service::setenv_attrubute(Message *j, string username)
{
    if (username != "")
    {
        j->msg["ENV"]["USER"] = username;
        return;
    }
    //string env_need[]={"HOME","HOSTNAME","PATH","TZ","USER","SHELL"};
    string env_need[] = {"HOME", "HOSTNAME", "PATH", "USER"};
    char *envdata;
    for (int i = 0; i < (int)(sizeof(env_need) / sizeof(env_need[0])); i++)
    {
        if ((envdata = getenv(env_need[i].c_str())))
        {
            j->msg["ENV"][env_need[i]] = envdata;
        }
    }
}

void subjob_service::parse_script(Message *j, string script_name)
{
    struct stat statbuf;
    if (stat(script_name.c_str(), &statbuf) < 0)
    {
        cout << "Subjob ---> parse_script(): ERROR! Script file cannot be loaded." << endl;
        exit(1);
    }

    if (!S_ISREG(statbuf.st_mode))
    {
        cout << "Subjob ---> parse_script(): ERROR! Script not a file." << endl;
        exit(1);
    }

    ifstream f(script_name);
    string line;
    int count = 0;
    while (getline(f, line))
    {
        line = trim(line);
        stringstream ss;
        ss << line;
        string set;
        ss >> set;
        if (set.compare("#SET") == 0)
        {
            while (ss >> set)
            {
                if (set.compare("-N") == 0)
                {
                    if (j->msg.count("NODENEED") == 0)
                    {
                        int nodeneed;
                        ss >> nodeneed;
                        j->msg["NODENEED"] = nodeneed;
                    }
                    else
                    {
                        cout << "Subjob ---> parse_script(): ERROR! SET multiple -N." << endl;
                        exit(1);
                    }
                }
                else if (set.compare("-P") == 0)
                {
                    if (j->msg.count("NPNEED") == 0)
                    {
                        int npneed;
                        ss >> npneed;
                        j->msg["NPNEED"] = npneed;
                    }
                    else
                    {
                        cout << "Subjob ---> parse_script(): ERROR! SET multiple -P." << endl;
                        exit(1);
                    }
                }
                else
                {
                    cout << "Subjob ---> parse_script(): ERROR! SET command wrong." << endl;
                    exit(1);
                }
            }
        }
        else if (set.compare("#MPI") == 0)
        {
            j->msg["MPI"] = true;
        }
        else
        {
            j->msg["SCRIPT"][count] = line;
            count++;
        }
    }
    if (j->msg.count("SCRIPT") == 0)
    {
        cout << "Error! Script file not has any command." << endl;
        exit(1);
    }
    if (j->msg.count("MPI") == 1)
    {
        if ((int)j->msg["SCRIPT"].size() > 1)
        {
            cout << "Error! MPI support only can have one command." << endl;
            exit(1);
        }
    }
    j->msg["JOBNAME"] = script_name + "@" + j->msg["ENV"]["USER"].get<string>();
}

void subjob_service::creatjob(Message *j, string script_name, string username)
{
    setenv_attrubute(j, username);
    parse_script(j, script_name);
    string sender("subjob");
    string receiver("server");
    string request("newjob");
    j->encode_Header(sender, receiver, request);
}
