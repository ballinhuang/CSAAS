#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "job_info.h"
using namespace std;

#define MaxConnectQuantity 3
#define CmdLen 1000

int test_counter = 0;
bool do_scheduler;
bool do_accept;
char s_ip[15];
char s_port[10];
char c_ip[15];
char c_port[10];
pthread_mutex_t lock;
vector<job_info> job_list;

int received_qsub(char *message, int fd) {
	char fileName[100];
	job_info newjob;
	pthread_mutex_lock(&lock);
	newjob.setjobid(++test_counter);
	sprintf(fileName,"test_output_%d", test_counter);
	pthread_mutex_unlock(&lock);
	FILE *file_ptr = fopen(fileName, "w");
	fprintf(file_ptr, "client_sock_fd = %d\n", fd);
	fprintf(file_ptr, "%s\n", message);
	fclose(file_ptr);
	cout << "qsub finished !" << endl;
	
	newjob.readdata(message);

	pthread_mutex_lock(&lock);
	job_list.push_back(newjob);
	pthread_mutex_unlock(&lock);

	pthread_mutex_lock(&lock);
	do_scheduler = true;
	pthread_mutex_unlock(&lock);
}

void req_runjob(){
	cout << "connect mom\n";
	int sock_fd = 0;
	struct sockaddr_in client_addr, server_addr;
	
	in_addr_t server_ip = inet_addr("127.0.0.1");
	in_port_t server_port = atoi("11302");

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("server client socket creat error !");
        exit(EXIT_FAILURE);
    }

    bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    server_addr.sin_addr.s_addr = server_ip;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi("11302"));

    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("server client socket connect error !\n");
        exit(EXIT_FAILURE);
    }
    string strbuf="";
    pthread_mutex_lock(&lock);
    job_list[0].parse_job_tomom(&strbuf);
    job_list.erase (job_list.begin());
    pthread_mutex_unlock(&lock);
    char buf[1000];
    strcpy(buf, strbuf.c_str());
    write(sock_fd,buf,CmdLen);
    cout << "write cmd to mom finished\n";
    close(sock_fd);
}

void findBufType(int client_fd, char *buf) {
	char *type = strstr(buf, "TYPE");
	for( ; *type != '='; type++);
	for(++type ; *type == ' '; type++);

	string s = "";
	for( ; *type != '\n' && *type != ' '; type++)
		s += *type;

	if(s == "QSUB")
	{
		received_qsub(buf, client_fd);
	}
	else if(s == "SCHEDULER")
	{
		type = strstr(buf, "SUBTYPE");
		for( ; *type != '='; type++);
		for(++type ; *type == ' '; type++);

		s = "";
		for( ; *type != '\n' && *type != ' '; type++)
			s += *type;
		if (s == "RUNJOB")
		{
			req_runjob();
		}
	}
	else
		cout << "error type\n";	
}

void *handle_scheduler_contact(void *arg)
{

	cout << "connect scheduler\n";
	int sock_fd = 0;
	struct sockaddr_in client_addr, server_addr;
	
	in_addr_t server_ip = inet_addr(c_ip);
	in_port_t server_port = atoi(c_port);

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("server client socket creat error !");
        exit(EXIT_FAILURE);
    }

    bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    server_addr.sin_addr.s_addr = server_ip;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(c_port));

    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("server client socket connect error !\n");
        exit(EXIT_FAILURE);
    }
    char *cmd = "do_scheduler";
    write(sock_fd,cmd,CmdLen);
    cout << "write cmd to scheduler finished\n";
    close(sock_fd);
    cout << "connect scheduler finished\n";
}

void accrpt_listener_cleanup(void *arg) {
	do_accept = false;
	cout << "cleanup\n";
}

void *start_accept_listener(void *arg) {
	pthread_cleanup_push(accrpt_listener_cleanup, NULL);

	int sock_fd = 0;
	struct sockaddr_in server_addr;

	in_addr_t server_ip = inet_addr(s_ip);
	in_port_t server_port = atoi(s_port);

	if( (sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) == -1) {
		cout << "server socket creat error !" << endl;
		exit(EXIT_FAILURE);
	}
	
	bool flag = true;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    
    memset(&server_addr, '\0', sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(s_port));
    server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        cout << "server bind error !" << endl;
        exit(EXIT_FAILURE);
    }

	if(listen(sock_fd, MaxConnectQuantity) == -1) {
        cout << "server listen error !" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "server waiting for accept\n";
    int client_sock_fd = 0, sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;	
	if((client_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1) {
            cout << "server accept failed !" << endl;
            continue;
    }
    cout << "server accept finished !\n";

	char buf[1000];
    read(client_sock_fd, buf, 1000);
    findBufType(client_sock_fd, buf);
    close(client_sock_fd);
    close(sock_fd);

    pthread_cleanup_pop(1);
}

void start_accept_thread() {
	pthread_t threadAccept;

	cout << "new thread !\n";	
	do_accept = true;
	if(pthread_create(&threadAccept, NULL, &start_accept_listener, NULL) != 0) {
		cout << "creat Accept thread error !" << endl;
		exit(EXIT_FAILURE);
	}
}

void monitor_accept_thread() {
	if(!do_accept)
		start_accept_thread();
}

void main_loop() {
	if(pthread_mutex_init(&lock, NULL) != 0)
	{
		cout << "mutex initialize error !" << endl;
		exit(EXIT_FAILURE);
	}

	start_accept_thread();

	while(1) {
		pthread_mutex_lock(&lock);

		if(do_scheduler) {
			//pthread_mutex_unlock(&lock);

			pthread_t threadScheduler;
			if(pthread_create(&threadScheduler, NULL, &handle_scheduler_contact, NULL) != 0) {
				cout << "creat Scheduler thread error !" << endl;
				exit(EXIT_FAILURE);
			}
			do_scheduler = false;
			pthread_mutex_unlock(&lock);
		}
		else
			pthread_mutex_unlock(&lock);

		monitor_accept_thread();
	}
}

void analyzeIPPort(const char *search, char *IPPort, char *base) {
	char *bufPtr;
	int index = 0;

	if((bufPtr = strstr(base, search)) != NULL) {
		for(bufPtr += strlen(search); *bufPtr != ' '; bufPtr++);
		bufPtr++;
		strcpy(IPPort, bufPtr);
		index = strlen(IPPort) - 1;
		if(IPPort[index] == '\n')
			IPPort[index] = '\0';
	}
}

void readIPPort() {
	FILE *ptr;
	char buf[1000] = {0};

	// Read IP of server and scheduler
	ptr = fopen("../config/IPConfig.txt", "r");
	if(ptr == NULL) {
		cout << "IPConfig doesn't exit !" << endl;
		exit(EXIT_FAILURE);
	}

	while(fgets(buf, sizeof(buf), ptr)) {
		analyzeIPPort("server", s_ip, buf);
		analyzeIPPort("scheduler", c_ip, buf);
	}
	fclose(ptr);

	// Read port of server and scheduler
	ptr = fopen("../config/PortConfig.txt", "r");
	if(ptr == NULL) {
		cout << "PortConfig doesn't exit !" << endl;
		exit(EXIT_FAILURE);
	}

	while(fgets(buf, sizeof(buf), ptr)) {
		analyzeIPPort("server", s_port, buf);
		analyzeIPPort("scheduler", c_port, buf);
	}
	fclose(ptr);
}

int main(int argc, char *argv[]) {
	readIPPort();

	pid_t pid= fork();
	if(pid == -1) {
    	cout << "server fork error !" << endl;
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) {
    	cout << "shut down parent !" << endl;
    	return 0;
    }

    do_accept = false;
    do_scheduler = false;

    main_loop();

	return 0;
}