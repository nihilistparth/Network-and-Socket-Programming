// Name : Parth tusham      19CS30034   
// Name : Shashwat Shukla   19CS10056
// Assignment 3

// Note IN mput and mget rather than input being like this
// mput <inp1>, <inp2> ..
// it is
// mput <inp1> <inp2> ..
// that is only separated by space and not with comma rest it works fine

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<fcntl.h> 
#include<errno.h> 
#include <ctype.h>
#include<stdio.h>
#include<string.h>//memset
#include<stdlib.h>//sizeof
#include<netinet/in.h>//INADDR_ANY

// #define PORT 8000
// #define SERVER_IP "127.0.0.1"
#define MAXSZ 200
#define BLOCK_SIZE 20

int check(char* a1, char* a2, int size){
    int i = 0 ; 
    for(i = 0 ; i < size ; i++){
        if(a1[i] != a2[i]){
            return 0 ; 
        }
    }
    return 1 ; 
}

int size_command(char* v){

    char* token = strtok(v, " ") ; 
    int j = 0 ; 
    while(token != NULL){
        j++ ; 
        token = strtok(NULL, " ") ; 
    }
    return j - 1 ; 
}

int change_dir(char inp[]){
    if(chdir(inp)!=0){
        return 0;
    }
    return 1;
}
int main(){
    int sockfd;//to create socket

    struct sockaddr_in serverAddress;//client will connect on this

    int n;
    char msg1[MAXSZ];
    char msg2[MAXSZ];

    //create socket
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    //initialize the socket addresses
    memset(&serverAddress,0,sizeof(serverAddress));
    //send to sever and receive from server
    
    char commandList[11][4] = {{"open"}, {"user"}, {"pass"}, {"cd"}, {"lcd"} , {"dir"}, {"get"}, {"put"}, {"mget"}, {"mput"}, {"quit"}} ; 
    int commandSize[11] = {4,4,4,2,3,3,3,3,4,4,4} ; 
    int i = 0 ; 
    while(1){
        
        printf("myFTP> ");
        fgets(msg1,MAXSZ,stdin);

        n = strlen(msg1)+1;

        int idx = -1 , k = 0; 
        for(k = 0 ; k < 11 ; k++){
            if(check(msg1, commandList[k], commandSize[k]) == 1){
                idx = k ; 
                break ; 
            }
        }

        if(idx == 10){
            printf("connection close\n") ; 
            break ; 
        }

        if(i == 0){
            if(idx == 0){
                
                int j = 0 ; 
                char SERVER_IP[100] ;
                int PORT ;  
                char* token = strtok(msg1, " ") ; 

                while(token != NULL){
                    if(j == 1){
                        PORT = atoi(token) ;          
                    }
                    else if(j == 2){
                        strcpy(SERVER_IP, token) ;
                    }
                    j++ ; 
                    token = strtok(NULL, " ") ;
                }
                
                serverAddress.sin_family=AF_INET;
                serverAddress.sin_addr.s_addr=inet_addr(SERVER_IP);
                serverAddress.sin_port=htons(PORT);

                int c = connect(sockfd,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
                if(c >= 0){
                    printf("Connection sucessfull\n") ; 
                    i++ ; 
                    continue;
                }
                else{
                    printf("\nConnection Failed \n");
                    return -1;
                }
            }
            else{
                printf("Please enter the valid first command/n") ; 
                continue ; 
            }
        }
        else{

            if(idx == 1){                       //  user 
                send(sockfd,msg1,n,0);
                n = recv(sockfd,msg2,MAXSZ,0);
            
            }
            else if(idx == 2){                  // pass 
                send(sockfd,msg1,n,0);
                n = recv(sockfd,msg2,MAXSZ,0);
            }
            else if(idx == 3){                  // cd
                send(sockfd,msg1,n,0);
                n = recv(sockfd,msg2,MAXSZ,0);
                // print the code recieved

            }
            else if(idx == 4){                  // lcd
                
                char inp[200];
                    int j =0;
                    for(int i = 4;i<200;i++){
                        if(msg1[i]=='\n' || msg1[i]=='\0'){
                            inp[j] = '\0';
                            break;
                        }
                        inp[j] =msg1[i];
                        j++;
                    }

                    char cwd[101];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("Current working dir: %s\n", cwd);
                    } else {
                        perror("getcwd() error");
                        return 1;
                    }
                    int ret_value  = change_dir(inp);
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("Current working dir: %s\n", cwd);
                    } else {
                        perror("getcwd() error");
                        return 1;
                    }
                    if(ret_value){
                       printf("The directory has been changed !\n") ; 
                    }
                    else{
                       printf("The directory has not been changed!\n");  
                    }

            }
            else if(idx == 5){                  // dir
                send(sockfd,msg1,n,0);
                // directory send files one by one from the server 
                char dummy[10] = "this is" ; 
                int k = recv(sockfd,msg2,MAXSZ,0);
                printf("the current content of directories\n") ; 
                msg2[k] = '\0' ; 
                while(k > 0){
                    printf("%s\n", msg2) ; 
                    send(sockfd, dummy, 10, 0) ; 
                    k = recv(sockfd, msg2, MAXSZ, 0) ;
                    msg2[k] = '\0' ;  
                    if(strcmp(msg2, "200") == 0){
                        break  ;
                    }
                    else if(strcmp(msg2, "500") == 0){
                        break ; 
                    }
                }
                printf("The server code recieved is %s\n", msg2) ; 
                continue ; 
            }
            else if(idx == 6){                  // get
            
                int i = 0 , sp = 0 , j = 0; 
                char local_file[200] ; 
                while(i<n){
                    if(isspace(msg1[i])){
                        sp++ ; 
                    }
                    else{
                        if(sp == 2){
                            local_file[j++] = msg1[i] ; 
                        }
                    }
                    i++ ; 
                }
                local_file[j] = '\0' ; 

                int fd = open(local_file, O_WRONLY | O_CREAT | O_TRUNC, 0644) ; 
                if(fd < 0){
                    perror("local file creation error\n") ; 
                    continue ; 
                }
                send(sockfd,msg1,n,0);

                n = recv(sockfd,msg2,MAXSZ,0);
                char dummy[10] = "this is" ; 

                if(strcmp(msg2, "200") == 0){
                    char headML[2] ;
                    send(sockfd, dummy, 10, 0) ;
                    int v = recv(sockfd, headML, 2, 0) ; 
                    headML[v] = '\0' ; 
                    while(strcmp(headML, "M")  == 0){
                        int sz = BLOCK_SIZE ; 
                        uint16_t szi ; 
                        send(sockfd, dummy, 10, 0) ;                    
                        recv(sockfd, &szi, sizeof(uint16_t), 0) ; 
                        char recvContents[sz+1] ;
                        bzero(recvContents, sizeof(recvContents)) ;
                        send(sockfd, dummy, 10, 0) ;    

                        v = recv(sockfd, recvContents, sz, 0) ; 
                        recvContents[v] = '\0' ;  
                        int siz = write(fd, recvContents, sz) ; 
                        send(sockfd, dummy, 10, 0) ; 
                        recv(sockfd, headML, 2, 0) ; 

                    }
                    int sz = BLOCK_SIZE ; 
                    char recvContents[sz+1] ; 
                    bzero(recvContents, sizeof(recvContents)) ; 
                    send(sockfd, dummy, 10, 0) ; 
                    v = recv(sockfd, recvContents, sz, 0) ;
                    recvContents[v] = '\0' ; 
                    int siz = write(fd, recvContents, v) ; 
                    close(fd); 
                    printf("Get file done successfully\n") ; 
                    
                }
                else{
                    printf("server side error\n") ; 
                }

            }
            else if(idx == 7){                  // put
                
                int i = 0 , sp = 0 , j = 0; 
                char local_file[200] ; 
                while(i<n){
                    if(isspace(msg1[i])){
                        sp++ ; 
                    }
                    else{
                        if(sp == 1){
                            local_file[j++] = msg1[i] ; 
                        }
                        else if(sp == 2){
                            break ; 
                        }
                    }
                    i++ ; 
                }
                local_file[j] = '\0' ; 
                printf("%s\n", local_file) ; 
                int fd = open(local_file, O_RDONLY) ; 

                if(fd < 0){
                    perror("local file not found\n") ; 
                    continue ; 
                }

                send(sockfd,msg1,n,0);
                recv(sockfd,msg2,MAXSZ,0);
                char dummy[10] ;

                if(strcmp(msg2, "200") == 0){
                    
                    char fileContents[BLOCK_SIZE+1] ; 
                    bzero(fileContents, sizeof(fileContents)) ; 
                    int sz = read(fd, fileContents, BLOCK_SIZE) ; 

                    while(sz == BLOCK_SIZE){
                        char tp[2] ; 
                        sprintf(tp, "M") ; 
                        tp[1] = '\0' ; 
                        send(sockfd, tp, 2, 0) ; 
                        recv(sockfd, dummy, 10, 0) ; 
                        send(sockfd, fileContents, BLOCK_SIZE, 0) ;
                        recv(sockfd, dummy, 10, 0) ;    
                        
                        sz = read(fd, fileContents, BLOCK_SIZE) ;
                    }

                    char tp[2] ; 
                    sprintf(tp, "L") ; 
                    tp[1] = '\0' ;
                    send(sockfd, tp, 2, 0) ; 
                    recv(sockfd, dummy, 10, 0) ;
                    
                    fileContents[sz] = '\0' ;
                    recv(sockfd, dummy, 10, 0) ;
                    send(sockfd, fileContents, sz, 0) ; 
                    
                    close(fd) ; 
                    printf("File sent successfully\n") ; 
                    continue ; 
                }
                else{
                    printf("server side error\n") ;
                    continue ; 
                }
                
            }

            else if(idx == 8){                  // mget
                
                int i = 0 ,sp = 0 , j = 0 ;
                char local_file[200] ; 
                send(sockfd,msg1,n,0);
                while(i < n){
                    if(isspace(msg1[i])){
                        sp++ ;
                        if(sp > 1){
                            local_file[j] = '\0' ; 
                            int fd = open(local_file, O_WRONLY | O_CREAT | O_TRUNC, 0644) ; 
                            if(fd < 0){
                                printf("client side file problem\n") ; 
                                break ;  
                            }
                            if(j == 0){
                                printf("Files done !\n") ; 
                                break ; 
                            }
                            send(sockfd,local_file,j,0);
                            n = recv(sockfd,msg2,MAXSZ,0);
                            char dummy[10] = "this is" ; 

                            if(strcmp(msg2, "200") == 0){
                                char headML[2] ;
                                send(sockfd, dummy, 10, 0) ;
                                int v = recv(sockfd, headML, 2, 0) ; 
                                headML[v] = '\0' ; 
                                while(strcmp(headML, "M")  == 0){
                                    int sz = BLOCK_SIZE ; 
                                    uint16_t szi ; 
                                    send(sockfd, dummy, 10, 0) ;                    
                                    recv(sockfd, &szi, sizeof(uint16_t), 0) ; 
                                    
                                    char recvContents[sz+1] ;
                                    bzero(recvContents, sizeof(recvContents)) ;
                                    
                                    send(sockfd, dummy, 10, 0) ;    
                                    v = recv(sockfd, recvContents, sz, 0) ; 
                                    recvContents[v] = '\0' ;  
                                    int siz = write(fd, recvContents, sz) ; 
                                    
                                    send(sockfd, dummy, 10, 0) ; 
                                    recv(sockfd, headML, 2, 0) ; 

                                }
                                int sz = BLOCK_SIZE ; 
                                char recvContents[sz+1] ; 
                                bzero(recvContents, sizeof(recvContents)) ; 
                                send(sockfd, dummy, 10, 0) ; 
                                v = recv(sockfd, recvContents, sz, 0) ;
                                recvContents[v] = '\0' ; 
                                int siz = write(fd, recvContents, v) ; 
                                close(fd); 
                                printf("Get file done successfully\n") ; 
                            }
                            else{
                                printf("server side error\n") ; 
                                break ; 
                            }

                        }
                        j = 0 ; 
                        bzero(local_file, sizeof(local_file)) ;
                    }   

                    else{
                        if(sp > 0){
                            local_file[j++] = msg1[i] ; 
                        }
                    }
                    i++ ; 
                }

            }
            else if(idx == 9){                  // mput
                
                int i = 0 ,sp = 0 , j = 0 ;
                char local_file[200] ; 
                bzero(local_file, sizeof(local_file)) ; 
                send(sockfd,msg1,n,0);
                while(i < n){
                    if(isspace(msg1[i])){
                        sp++ ;
                        if(sp > 1){
                            local_file[j] = '\0' ; 
                            int fd = open(local_file,  O_RDONLY) ; 
                            if(fd < 0){
                                printf("client side file problem\n") ; 
                                break ;  
                            }
                            if(j == 0){
                                printf("Files done !\n") ; 
                                break ; 
                            }
                            send(sockfd,local_file,j,0);
                            n = recv(sockfd,msg2,MAXSZ,0);
                            char dummy[10] ; 

                            if(strcmp(msg2, "200") == 0){
                    
                                char fileContents[BLOCK_SIZE+1] ; 
                                bzero(fileContents, sizeof(fileContents)) ; 
                                int sz = read(fd, fileContents, BLOCK_SIZE) ; 

                                while(sz == BLOCK_SIZE){
                                    char tp[2] ; 
                                    sprintf(tp, "M") ; 
                                    tp[1] = '\0' ; 
                                    send(sockfd, tp, 2, 0) ; 
                                    recv(sockfd, dummy, 10, 0) ; 
                                    send(sockfd, fileContents, BLOCK_SIZE, 0) ;
                                    recv(sockfd, dummy, 10, 0) ;    
                                    
                                    sz = read(fd, fileContents, BLOCK_SIZE) ;
                                }

                                char tp[2] ; 
                                sprintf(tp, "L") ; 
                                tp[1] = '\0' ;
                                send(sockfd, tp, 2, 0) ; 
                                recv(sockfd, dummy, 10, 0) ;
                                
                                fileContents[sz] = '\0' ;
                                
                                send(sockfd, fileContents, sz, 0) ; 
                                recv(sockfd, dummy, 10, 0) ;
                                close(fd) ; 
                                printf("File sent successfully\n") ;
                                bzero(local_file, sizeof(local_file)) ;
                                // i++; 
                                // continue ; 
                            }
                            else{
                                printf("server side error\n") ; 
                                break ; 
                            }
                             
                        }
                        j = 0 ; 
                        bzero(local_file, sizeof(local_file)) ;
                    }   

                    else{
                        if(sp > 0){
                            local_file[j++] = msg1[i] ; 
                        }
                    }
                    i++ ; 
                }

            }

            else{
                printf("Please enter a valid command\n") ; 
                continue ;
            }

        }

        printf("Receive message from  server::%s\n",msg2);
    }

    return 0;
}