#ifndef RSOCKET_H
#define RSOCKET_H

/*
Parth Tusham
Shashwat Shukla
Group 23
*/
/****************************************************************/

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <time.h>  
#include <unistd.h>  
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <pthread.h> 
#include <signal.h>
#include <assert.h>

/****************************************************************/

#define T 2 
#define p 0.50
#define MAX_TABLE_SIZE 150
#define SOCK_MRP 260
#define MAX_MSG_SIZE 100

/****************************************************************/

int r_socket(int domain, int type, int protocol) ; 

int r_bind(int socket, const struct sockaddr *address, socklen_t address_len);

ssize_t r_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t r_recvfrom(int sockfd, char *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

int r_close(int fd);

void* runnerR(void* param) ; 

void* runnerS(void* param) ; 

int dropMessage() ; 

/****************************************************************/

#endif