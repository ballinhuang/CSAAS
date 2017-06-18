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
int main(int arg,char **args)  
{  
	int len; 
	char buffer[1024] = "";
    char *argv[]{"ls","-l",NULL};
  	FILE *ptr;
    char *envp[]={0,NULL};
  // Read port of server and scheduler
	ptr = fopen("job.sh", "r");
	if(ptr == NULL) {
		cout << "PortConfig doesn't exit !" << endl;
		exit(EXIT_FAILURE);
	}

   
   int pfd[2];
    int status;
    pid_t pid;

    /* create pipe */
    if (pipe(pfd)<0)
        return -1;

    /* fork to execute external program or scripts */
    pid = fork();
    if (pid<0) {
        return 0;
    } else if (pid==0) { /* child process */
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);

        /* execute CGI */
        execvp( "ls" , argv);
        exit(0);
    } else { /* parent process */
        close(pfd[1]);
        /* print output from CGI */
        while((len=read(pfd[0], buffer, 1023))>0) {
            buffer[len] = '\0';
            printf("%s\n", buffer);
        }

        /* waiting for CGI */
        //waitpid((pid_t)pid, &status, 0);
    }
}  