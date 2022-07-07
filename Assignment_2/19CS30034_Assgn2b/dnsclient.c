// A Simple Client Implementation
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <errno.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define MAXLINE 100
#define BUFFSIZE 10
int main(int argc, char const *argv[]) { 
    int sockfd; 
    struct sockaddr_in servaddr; 
    int fd = 0; 
    char buffer[105];

    // Creating socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(8181); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n;
    socklen_t len; 
    char *hello = "CLIENT:HELLO"; 
      
    printf("Enter the DNS name \n");
    scanf("%s",buffer);
    int valread = 0;
    valread = sendto(sockfd, (const char *)buffer, strlen(buffer), 0, 
        (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("SENDING..... \n");
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd,&readfds);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 2000000;
    int val =  select(sockfd+1,&readfds,NULL,NULL,&tv);
    if(val<0){
        printf("UNSUCCESSFULL!\n");
        return -1;
    }
    if(val==0){
        printf("TIME LIMIT OF 2 SEC EXPIRED!\n");
        return -1;
    }
	

    valread = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &servaddr, &valread); 
    buffer[valread] = '\0';
	printf("INFORMATION RECIEVED BACK IS ");
    printf("%s \n",buffer);
    close(sockfd); 
    return 0; 
} 