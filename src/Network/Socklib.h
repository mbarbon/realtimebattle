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


#ifndef __SOCKLIB__
#define __SOCKLIB__

#ifdef  __cplusplus
extern "C" {
#endif

void inc_counter(int unused);

void acceptdeath(int unused);

void goaway(int signum);

void murderchildren(int unused);

int make_socket(struct sockaddr_in name);

int init_sockaddr(struct sockaddr_in *name, const char *hostname, unsigned short int port);

int server_init(char* myname, int lport, int uport, void (*handler)(int), int dbug);

int make_connect_socket(unsigned short int port, struct sockaddr_in name);

int connectto(char* chost, int myport, int dbug);

int unconnectto(int fd);

#ifdef  __cplusplus
}
#endif

#endif __SOCKLIB__
