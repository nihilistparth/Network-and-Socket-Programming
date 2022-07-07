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
    const char *file_name = argv[1];
    char buffer[105];
	if((fd = open(file_name,O_RDONLY))<0){
		printf("File %s not found ! \n",file_name);
		return -1;
	}
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
      
  
    int valread = 0;
    while((valread = read(fd,buffer,BUFFSIZE))>0){ // reading file
		if(valread<=0)  break;
        buffer[valread] = '\0';

		printf("SENDING..... \n");
		valread = sendto(sockfd, (const char *)buffer, strlen(buffer), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr)); // sending in chunk size 
	

		valread = recvfrom(sockfd, (char *)buffer, MAXLINE, 0,     
			( struct sockaddr *) &servaddr, &valread);              // recieving data from server
		buffer[valread] = '\0';
		
		
	}
    bzero(buffer,sizeof(buffer));
    valread = sendto(sockfd, (const char *)buffer, strlen(buffer), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr));

    valread = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &servaddr, &valread); 
    buffer[valread] = '\0';
    printf("File sent \n");
	printf("INFORMATION RECIEVED BACK IS ");
    printf("%s \n",buffer);
    close(sockfd); 
    return 0; 
} 