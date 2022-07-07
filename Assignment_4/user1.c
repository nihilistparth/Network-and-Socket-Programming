#include <stdio.h> 
#include <string.h>
#include <arpa/inet.h>
 
#include "rsocket.h"

#define MAXLINE 100 
#define ROLLNO 10056
/*
Parth Tusham
Shashwat Shukla
Group 23
*/
struct sockaddr_in m2_addr; 
socklen_t len; 
char buffer[MAXLINE] ;

int main(int argc,  char **argv ) { 
  
    // Creating socket file descriptor 
    srand(0) ; 
    // printf("y1\n") ; 
    int sockfd = r_socket(AF_INET, SOCK_MRP, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(1); 
    } 

    printf("Enter the text want to send\n") ; 

    scanf("%s",buffer);

    memset(&m2_addr, 0, sizeof(m2_addr)); 

    m2_addr.sin_family = AF_INET; 
    m2_addr.sin_port = htons(5000+2*ROLLNO); 
    m2_addr.sin_addr.s_addr = INADDR_ANY; 

    // printf("%d\n", (int )strlen(buffer)) ; 

    for(int i = 0; i < strlen(buffer) ; i++){
         
        // printf("%d %c\n", i, *(buffer + i)) ;   
        
        len = sizeof(m2_addr); 
        
        int r = r_sendto(sockfd, (const char* )(buffer + i), 1, 0,(const struct sockaddr *) &m2_addr, len);
        sleep(4) ; 
        // printf("%d, r value\n", r) ; 
    }

    // printf("y2\n") ; 
    // r_close(sockfd); 
    return 0; 

} 