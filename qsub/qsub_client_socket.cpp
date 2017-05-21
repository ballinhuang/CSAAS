#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "qsub_client_socket.h"
using namespace std;

void qsub(string job_data) {
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
    strcpy(buf, job_data.c_str());
    write(sock_fd,buf,1000);

    close(sock_fd);
}