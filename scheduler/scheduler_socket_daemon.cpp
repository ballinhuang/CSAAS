#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using namespace std;

#define Port 1290
#define CmdLen 1000

void runJob(char * cmd) {
	int sock_fd = 0;
    struct sockaddr_in client_addr, server_addr;
    
    in_addr_t server_ip = inet_addr("127.0.0.1");
    in_port_t server_port = atoi("1289");
    in_addr_t client_ip = inet_addr("127.0.0.1");
    in_port_t client_port = atoi("1280");

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "client socket creat error !" << endl;
        exit(1);
    }

    bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    client_addr.sin_addr.s_addr = client_ip;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(client_port);

    server_addr.sin_addr.s_addr = server_ip;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "client socket connect error !" << endl;
        exit(1);
    }

    char buf[1000];
    strcpy(buf, "TYPE = SCHEDULER\n");
    write(sock_fd,buf,1000);

    close(sock_fd);
}

void analyzeCmd(char *cmd) {
	// do nothing
	printf("scheduler print the command : %s\n", cmd);
}

void getServerCommand() {
	int sock_fd = 0;
	struct sockaddr_in scheduler_addr;

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1) {
		printf("scheduler socket creat error !\n");
		exit(1);
	}
	
	bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    
    memset(&scheduler_addr, '\0', sizeof(struct sockaddr_in));
    scheduler_addr.sin_family = AF_INET;
    scheduler_addr.sin_port = htons(Port);
    scheduler_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_fd, (struct sockaddr *)&scheduler_addr, sizeof(struct sockaddr)) == -1) {
        printf("scheduler bind error !\n");
        exit(1);
    }

	if(listen(sock_fd, 1) == -1) {
        printf("scheduler listen error !\n");
        exit(1);
    }
	
	int server_sock_fd = 0, sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr;

	while(1){
		char cmd[CmdLen] = {0};
        server_sock_fd = 0;
		printf("scheduler is waiting for server's command !\n");
		while((server_sock_fd = accept(sock_fd, (struct sockaddr *)&server_addr, (socklen_t*)&sin_size)) == -1)
	        printf("server accept failed !\n");
    	read(server_sock_fd, cmd, CmdLen);
    	close(server_sock_fd);
    	analyzeCmd(cmd);
    	runJob(cmd);
    }
}

int main(int argc, char *argv[]) {
	pid_t pid = fork();
	if(pid == -1) {
		printf("scheduler fork error !\n");
		exit(1);
	}
	else if(pid != 0) {
		printf("close scheduler parent process !\n");
		exit(1);
	}

	getServerCommand();
	
	return 0;
}