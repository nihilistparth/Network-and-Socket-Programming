// A Simple UDP Server that sends a HELLO message
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <ctype.h>
#include <netdb.h>
#define MAXLINE 100
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Create socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(8181); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    printf("\nServer Running....\n");
  
    int n; 
    socklen_t len;
    char buffer[MAXLINE]; 
 
    len = sizeof(cliaddr); 
    char msg[1000];
    int valread=0;
    char* IPbuffer;
    char tempIP[500];
    while(1){
        valread = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &cliaddr, &len); 

        buffer[valread] = '\0';
        printf("RECIEVING IN PROGRESS ..... \n");
        if(valread>0){
            // sending data back to client
            // printf("yes1\n");
            struct hostent *lh ;
            if((lh = gethostbyname(buffer))==NULL){
                 sprintf(msg,"the IP address could not be found !");
                 sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
                 continue; 
            }
            // printf("yes");
            if (lh)
                puts(lh->h_name);
            else{
                // printf("Parth");
                herror("gethostbyname");
            }
            struct in_addr **addr_list = (struct in_addr **)lh->h_addr_list;
			strcpy(tempIP, inet_ntoa(*addr_list[0]));

			for (int i = 1; addr_list[i] != NULL; i++) {
				strcat(tempIP, ", ");
				strcat(tempIP, inet_ntoa(*addr_list[i]));
			}
            // IPbuffer = inet_ntoa(*((struct in_addr*)
            //                lh->h_addr_list[0]));
            
            if("0.0.0.0" == tempIP){
                sprintf(msg,"the IP address could not be resolved !");
            }
            else{
                sprintf(msg,"the IP address(es) is/are %s",tempIP);
            }            
            sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
        }
        else{
            //sending data back to client
             sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
             printf("********RECIEVING COMPLETED******** \n");
        } 
    }
      
    return 0; 
} 