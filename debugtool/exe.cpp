/* getenv example: getting path */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* getenv */
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
void sig_fork(int signo)
{
    pid_t pid;
    int stat;
    printf("signal, ID is %d\n", getpid());
    pid=waitpid(0,&stat,WNOHANG);
    
    return;
}

int main ()
{
    pid_t p;
    p = fork();  // 執行完這行,就會產生出一個child process
    if (p < 0)            //表示有錯誤發生
        printf("error in fork!");
    else if(p > 0)
        exit(1);

  char* pPath;
  pPath = getenv ("PATH");
  if (pPath!=NULL)
    printf ("The current path is: %s\n",pPath);
    char *ls = "/ls";
    char *path = strtok(pPath,":");
    char *pa;
    char *buf;
    while(path != NULL){
        buf = (char*)malloc(sizeof(char) * ((strlen(path))+100) );
        memset(buf,0,(strlen(path))+100);
        strcpy(buf,path);
        strcat(buf,ls);
        if( access(buf, X_OK) == 0 ){
            break;
        }
        path = strtok(NULL,":");
    }
    printf("%s\n",buf);

    pid_t pid_test;
    pid_test=fork();  // 執行完這行,就會產生出一個child process
    int status;
    signal (SIGCHLD, sig_fork); // 設置SIGCHLD, 並呼叫waitpid(), 捕獲子程序的返回狀態
    char * argv[ ]={"ls","-al","/etc/passwd",(char *)0};
    char * envp[ ]={"PATH=/bin",0};

    //這時就有parent process和child process同時在運行
    if (pid_test < 0)            //表示有錯誤發生
        printf("error in fork!");
    else if (pid_test == 0) // 表示child process執行成功
    {
        
        printf("I am the child process, ID is %d,%d\n", getpid(),pid_test);
        for(int i = 0 ; i < 3 ; i++){
            pid_t pid;
            pid = fork();  // 執行完這行,就會產生出一個child process
            int state;
            //這時就有parent process和child process同時在運行
            if (pid < 0)            //表示有錯誤發生
                printf("error in fork!");
            else if(pid == 0){
                printf("I am the job process, ID is %d,%d\n", getpid(),pid);
                execve(buf,argv,envp);
            }
            else{
                waitpid( pid, &state, 0 );
                sleep(5);
                printf("job %d complete\n", i);
            }
        }
        exit(1);
    }
    else
    {
        //waitpid( pid_test, &status, 0 );
        while( kill((pid_t)pid_test, 0) == 0);
        printf("ALL job complete\n");
    }                              // 表示parent process執行成功
    
    while(1);

    return 0;
}