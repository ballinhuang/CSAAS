#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <errno.h>

#define SERVER_PORT 12345

#define TRUE 1
#define FALSE 0

using namespace std;

main(int argc, char *argv[])
{
    int i, len, rc, on = 1;
    int listen_sd, max_sd, new_sd;
    int desc_ready, end_server = FALSE;
    int close_conn;
    char buffer[80];
    struct sockaddr_in addr;
    /*************************************************************/
    /* Create an AF_INET stream socket to receive incoming      */
    /* connections on                                            */
    /*************************************************************/
    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0)
    {
        perror("socket() failed");
        exit(-1);
    }
    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(-1);
    }
    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    rc = ioctl(listen_sd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(listen_sd);
        exit(-1);
    }
    /*************************************************************/
    /* Bind the socket                                           */
    /*************************************************************/
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SERVER_PORT);
    rc = bind(listen_sd,
              (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        perror("bind() failed");
        close(listen_sd);
        exit(-1);
    }
    /*************************************************************/
    /* Set the listen back log                                   */
    /*************************************************************/
    rc = listen(listen_sd, 32);
    if (rc < 0)
    {
        perror("listen() failed");
        close(listen_sd);
        exit(-1);
    }

    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    efd = epoll_create1(0);
    if (efd == -1)
    {
        perror("epoll_create");
        abort();
    }

    event.data.fd = listen_sd;
    event.events = EPOLLIN | EPOLLET;
    rc = epoll_ctl(efd, EPOLL_CTL_ADD, listen_sd, &event);

    if (rc == -1)
    {
        perror("epoll_ctl");
        abort();
    }

    /* Buffer where events are returned */
    events = (epoll_event *)calloc(256, sizeof event);

    do
    {
        printf("Waiting on epoll()...\n");
        rc = epoll_wait(efd, events, 256, -1);

        if (rc < 0)
        {
            perror("  epoll() failed");
            break;
        }
        desc_ready = rc;
        for (i = 0; i < desc_ready; ++i)
        {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                /* An error has occured on this fd, or the socket is not
                 ready for reading (why were we notified then?) */
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            }

            else if (events[i].data.fd == listen_sd)
            {
                /* We have a notification on the listening socket, which
                 means one or more incoming connections. */
                while (1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof in_addr;
                    infd = accept(listen_sd, &in_addr, &in_len);
                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            /* We have processed all incoming
                             connections. */
                            break;
                        }
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    rc = getnameinfo(&in_addr, in_len,
                                     hbuf, sizeof hbuf,
                                     sbuf, sizeof sbuf,
                                     NI_NUMERICHOST | NI_NUMERICSERV);
                    if (rc == 0)
                    {
                        printf("Accepted connection on descriptor %d "
                               "(host=%s, port=%s)\n",
                               infd, hbuf, sbuf);
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    rc = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if (rc == -1)
                    {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            }
            else
            {
                printf("  Descriptor %d is readable\n", i);
                close_conn = FALSE;
                do
                {
                    rc = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                    if (rc < 0)
                    {
                        if (errno != EAGAIN)
                        {
                            perror("  recv() failed");
                            //close_conn = TRUE;
                        }
                        break;
                    }

                    /**********************************************/
                    /* Check to see if the connection has been    */
                    /* closed by the client                       */
                    /**********************************************/
                    if (rc == 0)
                    {
                        printf("  Connection closed\n");
                        close_conn = TRUE;
                        break;
                    }

                    /**********************************************/
                    /* Data was received                          */
                    /**********************************************/
                    len = rc;
                    printf("  %d bytes received\n", len);

                    /**********************************************/
                    /* Echo the data back to the client           */
                    /**********************************************/
                    rc = send(events[i].data.fd, buffer, len, 0);
                    if (rc < 0)
                    {
                        perror("  send() failed");
                        close_conn = TRUE;
                        break;
                    }

                } while (TRUE);

                /*************************************************/
                /* If the close_conn flag was turned on, we need */
                /* to clean up this active connection.  This     */
                /* clean up process includes removing the        */
                /* descriptor from the master set and            */
                /* determining the new maximum descriptor value  */
                /* based on the bits that are still turned on in */
                /* the master set.                               */
                /*************************************************/
                if (close_conn)
                {
                    printf("Closed connection on descriptor %d\n",
                           events[i].data.fd);
                    close(events[i].data.fd);
                }
            } /* End of existing connection is readable */
        }     /* End of loop through selectable descriptors */

    } while (end_server == FALSE);

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    free(events);

    close(listen_sd);
}