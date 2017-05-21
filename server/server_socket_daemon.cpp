#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

#define ServerPort 1289
#define MaxConnectQuantity 3

int test_counter = 0;

// test for socket
    // there will product a file and its content was according to the client socket
int received_qsub(char *message, int fd) {
    char fileName[99];

    sprintf(fileName,"test_output_%02d", ++test_counter);
    FILE *file_ptr = fopen(fileName, "w");
    fprintf(file_ptr, "client_sock_fd = %d\n", fd);
    fprintf(file_ptr, "%s\n", message);

    fclose(file_ptr);
	cout << "qsub finished !" << endl;
}

void findBufType(int client_fd, char *buf) {
	char *type = strstr(buf, "TYPE");
	for( ; *type != '='; type++);
	for(++type ; *type == ' '; type++);

	string s = "";
	for( ; *type != '\n' && *type != ' '; type++)
		s += *type;

	if(s == "QSUB")
		received_qsub(buf, client_fd);
	else if(s == "SCHEDULER")
		;// do something
}

int main(int argc, char *argv[]) {
	int sock_fd = 0;
	struct sockaddr_in server_addr;

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1) {
		cout << "server socket creat error !" << endl;
		return 0;
	}
	
	bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    
    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ServerPort);
    server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "server bind error !" << endl;
        return 0;
    }

	if(listen(sock_fd, MaxConnectQuantity) == -1) {
        cout << "server listen error !" << endl;
        return 0;
    }

    pid_t pid= fork();
    if(pid == -1) {
    	cout << "server fork error !" << endl;
        return 0;
    }
    else if(pid != 0)
    	return 0;
	
	int client_sock_fd = 0, sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;

	cout << "server daemon starts to run in background !" << endl;
	while(1) {
		if((client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1) {
            cout << "server accept failed !" << endl;
            continue;
        }

		char buf[1000];
        read(client_sock_fd, buf, 1000);
        findBufType(client_sock_fd, buf);

        close(client_sock_fd);
	}
	
	return 0;
}