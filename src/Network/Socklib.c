/* Socklib.h 1999 Brian Mastenbrook
        Library for socket programming
        This file is free software, do what you please with it
        so long as keep this message intact and don't stop others
        from doing what they please with it. (aka copyleft)
        If you like, you can consider it GNU LGPL'ed, but without
        the legalese.
        You can also request to be denied the right to redistribute,
        copy, modify, and otherwise do anything but use this file
        (aka Microsoft EULA). Doing so will declare you stupid.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>


#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "Socklib.h"


#ifdef MSG_OOB
#undef MSG_OOB
#endif

#ifdef MSG_DONTROUTE
#undef MSG_DONTROUTE
#endif

#ifdef MSG_DONTWAIT
#undef MSG_DONTWAIT
#endif

#ifdef MSG_NOSIGNAL
#undef MSG_NOSIGNAL
#endif

#define MSG_OOB           0x1     /* process out-of-band data */
#define MSG_DONTROUTE     0x4     /* bypass routing, use direct */
#define MSG_DONTWAIT      0x40    /* don't block */
#define MSG_NOSIGNAL      0x2000  /* don't raise SIGPIPE */

int client_connections=0;
int total_connections=0;

void inc_counter(int unused)
{
        if (unused==SIGUSR1)
        {
                client_connections++;
                total_connections++;
        }
        else if (unused==SIGUSR2)
        {
                client_connections--;
        }
}

void acceptdeath(int unused)
{
        signal(unused,acceptdeath);
        wait(NULL);
}

void goaway(int signum)
{
        fprintf(stderr,"Recieved signal %d, sending children SIGUSR2 before going away.\n", signum);
        kill(0,SIGUSR2);
        exit(EXIT_SUCCESS);
}

void murderchildren(int unused)
{
        fprintf(stderr,"Recieved signal %d, murdering children before dying.\n",unused);
        kill(0,SIGTERM);
        exit(EXIT_SUCCESS);
}

int make_socket(struct sockaddr_in name)
{
        int sock;
        /* Create the socket. */
        sock=socket(PF_INET,SOCK_STREAM,0);
        if (sock<0)
        {
                perror("socket");
                return(-1);
        }
        /* Give the socket a name. */
        if (bind(sock,(struct sockaddr*)&name,(socklen_t) sizeof(name))<0)
        {
                perror("bind");
                return(-1);
        }
        return(sock);
}
int init_sockaddr(struct sockaddr_in *name, const char *hostname, unsigned short int port)
{
        struct hostent* hostinfo;
        name->sin_family=AF_INET;
        name->sin_port=htons(port);
        hostinfo=gethostbyname(hostname);
        if (hostinfo==NULL) 
        {
                fprintf(stderr,"Unknown host %s.\n",hostname);
                return(-1);
        }
        name->sin_addr=*(struct in_addr*)hostinfo->h_addr;
        return(0);
}

int server_init(char* myname, int lport, int uport, void (*handler)(int), int dbug)
{
        struct sockaddr_in myaddr;
        struct sockaddr_in theiraddr;
        int fd,newfd,tmp;
        size_t theirsize;
        int myport=lport;
        theirsize=sizeof(struct sockaddr_in);
        if (dbug==1)
        {
                fprintf(stderr,"Socklib.h version very low, 1999 Brian Mastenbrook.\n");
                fprintf(stderr,"Initializing...");
        }
        signal(SIGTERM,murderchildren);
        signal(SIGINT,murderchildren);
        signal(SIGCHLD,acceptdeath);
        signal(SIGUSR1,inc_counter);
        signal(SIGUSR2,inc_counter);
        for (tmp=lport;tmp<=uport;tmp++)
        {
                if (fork()==0)
                {
                        if (dbug==1)
                                fprintf(stderr,"[%d]",tmp);
                        myport=tmp;
                        break;
                }
                else if (tmp==uport)
                {
                        if (dbug==1)
                                fprintf(stderr,"[Parent die]done.\n");
                        return(1);
                }
        }
        signal(SIGTERM,SIG_DFL);
        signal(SIGINT,SIG_DFL);
        if (init_sockaddr(&myaddr,myname,myport)==-1)
        {
                return(-1);
        }
        fd=make_socket(myaddr);
        if (fd==-1)
        {
                return(-1);
        }
        if (dbug==1)
                fprintf(stderr,"done.\n");
        for (;;)
        {
                listen(fd,32);
                newfd=accept(fd,&theiraddr,(socklen_t*) &theirsize);
                if (fork()==0)
                {
                        signal(SIGPIPE,goaway);
                        kill(0,SIGUSR1);
                        if (dbug)
                                fprintf(stderr,"Connection from host: %s on port %d is # %d\n",inet_ntoa(theiraddr.sin_addr),myport,client_connections);
                        handler(newfd);
                        close(fd);
                        shutdown(fd,2);
                        if (dbug)
                                fprintf(stderr,"Connection closed, remote host %s port %d.\n",inet_ntoa(theiraddr.sin_addr),myport);
                        kill(0,SIGUSR2);
                        exit(0);
                }
        }
}

int make_connect_socket(unsigned short int port, struct sockaddr_in name)
{
        int sock;
        sock=socket(PF_INET,SOCK_STREAM,0);
        if (sock<0)
        {
                perror("socket");
                return(-1);
        }
        return(sock);
}

int connectto(char* chost, int myport, int dbug)
{
        struct sockaddr_in myaddr;
        int fd;
        if (init_sockaddr(&myaddr,chost,myport)==-1)
        {
                return(-1);
        }
        fd=make_connect_socket(myport,myaddr);
        if (fd==-1)
        {
                return(-1);
        }
        if (connect(fd,&myaddr,sizeof(struct sockaddr_in))<0)
        {
                perror("connect");
                return(-1);
        }
        signal(SIGPIPE,goaway);
        signal(SIGUSR2,SIG_IGN);
        if (dbug) printf("Connected to host: %s\n",inet_ntoa(myaddr.sin_addr));
        return(fd);
}

int unconnectto(int fd)
{
        return(shutdown(fd,2));
}
