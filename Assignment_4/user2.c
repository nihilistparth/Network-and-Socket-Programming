/*
Parth Tusham
Shashwat Shukla
Group 23
*/
#include <stdio.h> 
#include <string.h> 
#include <sys/types.h>
#include <arpa/inet.h>

#include "rsocket.h"


#define MAXLINE 100 
#define ROLLNO 10056


int main(int argc,  char **argv ) { 
  
    struct sockaddr_in m1_addr, m2_addr; 
    socklen_t len; 
    char buffer[MAXLINE] ;
    
    // printf("yes a\n") ; 
    memset(&m1_addr, 0, sizeof(m1_addr)); 
    memset(&m2_addr, 0, sizeof(m2_addr)); 

    // Creating socket file descriptor 
    int sockfd = r_socket(AF_INET, SOCK_MRP, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

    m2_addr.sin_family = AF_INET;
    m2_addr.sin_port = htons(5000 + 2 * ROLLNO);
    m2_addr.sin_addr.s_addr = INADDR_ANY; 

    // printf("yes b\n") ;

    if (r_bind(sockfd, (const struct sockaddr *)&m2_addr, sizeof(m2_addr)) < 0 ) { 
        perror("bind failed"); 
        // r_close(sockfd);
        exit(EXIT_FAILURE); 
    }
    
    len = sizeof(m1_addr);
    // printf("yes c\n") ;

    for(int i = 0;i<MAXLINE;i++){
        // printf("yes d\n") ;  
        int n = r_recvfrom(sockfd, (char *)buffer, MAXLINE, 0,  ( struct sockaddr *) &m1_addr, &len);
        printf("%s\n", buffer); 
        // fflush(stdout);
    }
    printf("\n");
    // r_close(sockfd); 
    return 0; 
} 