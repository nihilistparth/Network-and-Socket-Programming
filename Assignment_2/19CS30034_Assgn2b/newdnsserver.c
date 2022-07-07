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
    int sockfd, listenfd;
    pid_t childpid; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Create socket file descriptor 
    //UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    //TCP socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( sockfd < 0 ) { 
        perror("UDP socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    if(listenfd<0){
        perror("TCP socket creation error!");
        return -1;
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
    if(bind(listenfd,(const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed"); perror("bind failed"); 
        exit(EXIT_FAILURE); 
        exit(EXIT_FAILURE); 

    }
    listen(listenfd, 10);
    printf("\nServer Running....\n");
    fd_set rset;
    FD_ZERO(&rset);
    int maxfd = 0;
    if(listenfd>sockfd){
        maxfd = listenfd+1;
    }
    else{
         maxfd = sockfd+1;
    }

    int n; 
    socklen_t len;
    char buffer[MAXLINE]; 
 
    len = sizeof(cliaddr); 
    char msg[1000];
    char tempIP[500];
    int valread=0;
    char* IPbuffer;
    // char msg[100];
    while(1){
        FD_SET(listenfd,&rset);
        FD_SET(sockfd,&rset);
        int nready;
        nready = select(maxfd,&rset,NULL,NULL,NULL);
        if(FD_ISSET(listenfd,&rset)){
            int len = sizeof(cliaddr);
            int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
            if((childpid==fork())==0){
                // close(listenfd);
                bzero(buffer, sizeof(buffer));
                printf("Message recieved from TCP client \n");
                valread = recv(connfd, buffer, sizeof(buffer),0);
                buffer[valread] = '\0';
                // printf("%s \n",buffer);
                struct hostent *lh ;
                if((lh = gethostbyname(buffer))==NULL){
                    sprintf(msg,"the IP address could not be found !");
                    sendto(connfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
                    close(connfd);
                    continue; 
                }
                struct in_addr **addr_list = (struct in_addr **)lh->h_addr_list;
                strcpy(tempIP, inet_ntoa(*addr_list[0]));

                for (int i = 1; addr_list[i] != NULL; i++) {
                    strcat(tempIP, ", ");
                    strcat(tempIP, inet_ntoa(*addr_list[i]));
                }
            
                // IPbuffer = inet_ntoa(*((struct in_addr*)
                //                 lh->h_addr_list[0]));
                
                if("0.0.0.0" == tempIP){
                    sprintf(msg,"the IP address could not be found !");
                }
                else{
                    sprintf(msg,"the IP address is %s",tempIP);
                }            
                
                send(connfd, (const char*)msg, sizeof(msg),0);
                printf("********RECIEVING COMPLETED******** \n");
                close(connfd);
            }
             close(connfd);
        }
        if (FD_ISSET(sockfd, &rset)) {
            len = sizeof(cliaddr);
            bzero(buffer, sizeof(buffer));
            printf("\nMessage recieved from UDP client \n");
            
            n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                        (struct sockaddr*)&cliaddr, &len);
            
            
            struct hostent *lh ;
            if((lh = gethostbyname(buffer))==NULL){
                    printf("hemlo");
                    sprintf(msg,"the IP address could not be found !");
                    sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr));
                    continue; 
            }
            struct in_addr **addr_list = (struct in_addr **)lh->h_addr_list;
            strcpy(tempIP, inet_ntoa(*addr_list[0]));

            for (int i = 1; addr_list[i] != NULL; i++) {
                strcat(tempIP, ", ");
                strcat(tempIP, inet_ntoa(*addr_list[i]));
            }
            // IPbuffer = inet_ntoa(*((struct in_addr*)
            //                 lh->h_addr_list[0]));
            // printf("IP: %s \n",IPbuffer);
            if("0.0.0.0" == tempIP){
                sprintf(msg,"the IP address could not be found !");
            }
            else{
                // printf("yes2");
                sprintf(msg,"the IP address is %s",tempIP);
            }
             sendto(sockfd, (const char *)msg, strlen(msg), 0,(const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
             printf("********RECIEVING COMPLETED******** \n");
        }
    }
      
    return 0; 
} 