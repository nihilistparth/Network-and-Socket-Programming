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
    char msg[100];
    int valread=0,sentences=0,chars= 0,words=0;
    int cur_char =0;
    while(1){
        valread = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
			( struct sockaddr *) &cliaddr, &len); 

        buffer[valread] = '\0';
        printf("RECIEVING IN PROGRESS ..... \n");
        chars+=valread;
        if(valread>0){
            int i =0;
            // counting characters , words and sentences
            for(i=0;i<strlen(buffer);i++){
                char c = buffer[i];
                if(c=='\0'){
				}
                if(isspace(c)){
                    if(cur_char>0){
                        words++;
                    }
                    cur_char = 0;
                }
                if(c=='.'){
                    sentences++;
                    if(cur_char>0){
                        words++;
                    }
                    cur_char = 0;
                }
                if((c<='z' && c>='a') || (c<='Z' && c>='A')|| (c >= '0' && c <= '9')){
                    cur_char++;
				}
            }
            // sending data back to client
            sprintf(msg,"characters are %d ,words are %d and sentences are %d",chars,words,sentences);
            sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
        }
        else{
            //sending data back to client
             sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
             printf("********RECIEVING COMPLETED******** \n");
             words =0;sentences=0;chars =0;cur_char=0;
        } 
    }
      
    return 0; 
} 