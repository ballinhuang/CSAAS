#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
using namespace std;
int socket_server(int port)
{
    int sock_fd = 0;
	struct sockaddr_in server_addr;

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1) {
		cout << "server socket creat error !" << endl;
		exit(EXIT_FAILURE);
	}
	
	//bool flag = true;
    //setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    
    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "server bind error !" << endl;
        exit(EXIT_FAILURE);
    }

	if(listen(sock_fd, 3) == -1) {
        cout << "server listen error !" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "server waiting for accept\n";
    int client_sock_fd = 0, sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;	
	if((client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1) {
            cout << "server accept failed !" << endl;
    }
    cout << "server accept finished !\n";

    string result = "";
    char buf[100];
    while(read(client_sock_fd,buf,99)){
        result += buf;
        memset(buf, '\0', 100);
    }
    cout << result << endl;
    close(client_sock_fd);
    close(sock_fd);
}

int main(int argc, char *argv[])
{
    pid_t pid= fork();
	if(pid == -1) {
    	cout << "server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) {
    	cout << "shut down parent !" << endl;
    	return 0;
    }
    socket_server(5002);

    return 0;
}