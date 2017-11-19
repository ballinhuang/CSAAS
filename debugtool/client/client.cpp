#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> //for socket
#include <ctime>
#include <ThreadPool.h>
#include <vector>
#define PORT 12345 /* the port client will be connecting to */

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

int main(int argc, char *argv[])
{
    ThreadPool pool(10);
    std::vector<std::future<int>> results;

    /* sleep(5); */
    time_t t1 = time(NULL);

    for (int i = 0; i < 1; i++)
    {
        results.emplace_back(pool.enqueue([] {
            int sockfd,
                numbytes;
            char buf[MAXDATASIZE];
            struct hostent *he;
            struct sockaddr_in their_addr; /* connector's address information */

            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                perror("socket");
                exit(1);
            }

            their_addr.sin_family = AF_INET;   /* host byte order */
            their_addr.sin_port = htons(PORT); /* short, network byte order */
            their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */

            if (connect(sockfd, (struct sockaddr *)&their_addr,
                        sizeof(struct sockaddr)) == -1)
            {
                perror("connect");
                exit(1);
            }
            if (send(sockfd, "Hello, world!\n", 14, 0) == -1)
                perror("send");
            if (send(sockfd, "Hello, world2!\n", 14, 0) == -1)
                perror("send");
            if (send(sockfd, "Hello, world3!\n", 14, 0) == -1)
                perror("send");
            char buffer[80];
            recv(sockfd, buffer, sizeof(buffer), 0);
            puts(buffer);
            close(sockfd);
            return 0;
        }));
    }
    for (auto &&result : results)
        ;
    time_t t2 = time(NULL);
    printf("calculation time: %ld s\n", (t2 - t1));
    return 0;
}
