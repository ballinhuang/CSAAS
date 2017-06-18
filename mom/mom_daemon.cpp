#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

#define Port 11302
#define CmdLen 1000
int test_counter = 0;

void runJob(char * cmd) {
    /*char fileName[100];
    sprintf(fileName,"test_output_%d", ++test_counter);
    FILE *file_ptr = fopen(fileName, "w");
    fprintf(file_ptr, "%s", cmd);
    fclose(file_ptr);*/
    cout << "mom run finished !" << endl;
    char buffer[1024] = "";
    int len; 
    string user;
    string  exe;
    string argcstr;
    char *argc[10]={0};
    stringstream ss;
    string str(cmd);
    ss << str;
    string type;
    while(ss >> type){
    	if(type == "User")
    	{
    		ss >> type; // =
    		ss >> user;
    	}
    	if (type == "Script")
    	{
    		ss >> type; // =
    		ss >> exe;
    		argc[0] = (char*)malloc(sizeof(char)*exe.size());
    		strcpy(argc[0],exe.c_str());
    		int i = 1;
    		for (; ss >> argcstr && i<10 ; i++)
    		{
    			argc[i] = (char*)malloc(sizeof(char)*argcstr.size());
    			strcpy(argc[i],argcstr.c_str());
    			cout << argcstr << endl;    		}
    	}
    }

    int pid;
    int pfd[2];

    if(pipe(pfd)<0)
    {
    	cout << "pipe err";
    	exit(1);
    }

    pid = fork();

    if(pid == -1) {
		printf("mom fork error !\n");
		exit(1);
	}else if(pid == 0){
		dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);

        /* execute CGI */
        execvp( exe.c_str(), argc );
        exit(0);
	}else { /* parent process */
        close(pfd[1]);
        /* print output from CGI */
        while((len=read(pfd[0], buffer, 1023))>0) {
            buffer[len] = '\0';
            char fileName[100];
    		sprintf(fileName,"test_output_%d", ++test_counter);
    		FILE *file_ptr = fopen(fileName, "w");
    		fprintf(file_ptr, "%s", buffer);
    		fclose(file_ptr);
        }

    }
}


void getServerCommand() {
	int sock_fd = 0;
	struct sockaddr_in scheduler_addr;

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1) {
		printf("mom socket creat error !\n");
		exit(1);
	}
	
	bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    
    memset(&scheduler_addr, '\0', sizeof(struct sockaddr_in));
    scheduler_addr.sin_family = AF_INET;
    scheduler_addr.sin_port = htons(Port);
    scheduler_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_fd, (struct sockaddr *)&scheduler_addr, sizeof(struct sockaddr)) == -1) {
        printf("mom bind error !\n");
        exit(1);
    }

	if(listen(sock_fd, 1) == -1) {
        printf("mom listen error !\n");
        exit(1);
    }
	
	int server_sock_fd = 0, sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr;

	while(1){
		char cmd[CmdLen] = {0};
        server_sock_fd = 0;
		printf("mom is waiting for server's command !\n");
		while((server_sock_fd = accept(sock_fd, (struct sockaddr *)&server_addr, (socklen_t*)&sin_size)) == -1)
	        printf("mom accept failed !\n");
    	read(server_sock_fd, cmd, CmdLen);
    	close(server_sock_fd);
    	runJob(cmd);
    }
}

int main(int argc, char *argv[]) {
	pid_t pid = fork();
	if(pid == -1) {
		printf("mom fork error !\n");
		exit(1);
	}
	else if(pid != 0) {
		printf("close mom parent process !\n");
		exit(1);
	}

	getServerCommand();
	
	return 0;
}