// Name : Parth tusham      19CS30034   
// Name : Shashwat Shukla   19CS10056
// Assignment 3


// Note IN mput and mget rather than input being like this
// mput <inp1>, <inp2> ..
// it is
// mput <inp1> <inp2> ..
// that is only separated by space and not with comma rest it works fine

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <ctype.h>
#include <netdb.h>
#include <dirent.h> 
#include<fcntl.h> 

#include <arpa/inet.h>
#include<errno.h> 

#include<string.h>//memset
#include<netinet/in.h>//INADDR_ANY


#define PORT 8000
#define MAXSZ 100
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
int user(char inp[]){
    FILE* file = fopen("user.txt", "r");
    char line[256];
    // printf("user %s \n",inp);
    int j = 0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        int i = 0;
        while(line[i]!='\0' || line[i]!='\n'){
            if(isspace(line[i])){
                fclose(file);
                return 1;
            }
            if(line[i]!=inp[i]){
                fclose(file);
                return 0;
            }
            i++;
        }
        j++;
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
    // printf("user %s",inp);
    return 1;
}
int sz(char * x ,int i){
    int ans =0;
    while(x[i]!='\0' && x[i]!='\n'){
        ans++;
        i++;
    }
    return ans;
}
int check_pass(char* line,int i,char* inp){
    int n = sz(inp,0);
    int m = sz(line,i);
    // printf("%d, %d, %d \n",i,n,m);
    // printf("%s %s \n",line,inp);
    int j =0;
    if(m==n){
        while(line[i]!='\0' && line[i]!='\n'){
            // printf("%c  %c",line[i],inp[j]);
            if(line[i]!=inp[j]){
                return 0;
            }
            i++;
            j++;
        }
        return 1;
    }
   else  return 0;
}
int pass(char username[],char inp[]){
  FILE* file = fopen("user.txt", "r");
    char line[256];
    // printf("password %s \n",inp);
    int j = 0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        int i = 0;
        while(line[i]!='\0' && line[i]!='\n'){
            if(isspace(line[i])){
                int ret_value = check_pass(line,i+1,inp);
                fclose(file);
                return ret_value;
            }
            if(line[i]!=username[i]){
                fclose(file);
                return 0;
            }
            i++;
        }
        j++;
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
    // printf("user %s",inp);
    return 1;
}
int change_dir(char inp[]){
    if(chdir(inp)!=0){
        return 0;
    }
    return 1;
}
int main(){
    int sockfd;//to create socket
    int newsockfd;//to accept connection

    struct sockaddr_in serverAddress;//server receive on this address
    struct sockaddr_in clientAddress;//server sends to client on this address
    char commandList[9][4] = {{"open"}, {"user"}, {"pass"}, {"cd"}, {"dir"} ,{"get"}, {"put"}, {"mget"}, {"mput"}} ; 
    int commandSize[9] = {4,4,4,2,3,3,3,4,4} ; 
    int n;
    char msg[MAXSZ];
    int clientAddressLength;
    int pid;

    //create socket
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    //initialize the socket addresses
    memset(&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
    serverAddress.sin_port=htons(PORT);

    //bind the socket with the server address and port
    int b = bind(sockfd,(struct sockaddr *)&serverAddress, sizeof(serverAddress));

    //listen for connection from client
    listen(sockfd,5);

    while(1){
    //parent process waiting to accept a new connection
        printf("\n*****server waiting for new client connection:*****\n");
        clientAddressLength=sizeof(clientAddress);
        newsockfd=accept(sockfd,(struct sockaddr*)&clientAddress,&clientAddressLength);
        printf("connected to client: %s\n",inet_ntoa(clientAddress.sin_addr));
        //child process is created for serving each new clients
        pid = fork();
        if(pid==0)//child process rec and send
        {
        //rceive from client
            int op = 1 ; 
            while(1)
            {    
                n=recv(newsockfd,msg,MAXSZ,0);
                if(n==0)
                {
                    close(newsockfd);
                    break;
                }
                msg[n]=0;//command recieved
                char username[201]; //user 
              
                int idx = -1 , k = 0; 
                for(k = 0 ; k < 9 ; k++){
                    if(check(msg, commandList[k], commandSize[k]) == 1){
                        idx = k ; 
                        break ; 
                    }

                }

                int ret_value = 1;
            
                if(op==1){                                 // user
                    if(idx != 1){
                        sprintf(msg,"500");
                        send(newsockfd, msg, 4, 0) ; 
                        printf("Enter the username command\n") ; 
                        continue ;   
                    }
                    else{
                        char inp[200];
                        int j =0;
                        for(int i = 5;i<200;i++){
                            if(msg[i]=='\n' || msg[i]=='\0'){
                                inp[j] = '\0';
                                break;
                            }
                            inp[j] =msg[i];
                            j++;
                        }
                        ret_value  = user(inp);
                        if(ret_value){
                            // strcpy(username,inp);
                            // printf("user %s \n",username );
                            sprintf(msg,"200");
                            send(newsockfd, msg, 4, 0) ;
                            op++ ; 
                        }
                        else{
                            sprintf(msg,"500");
                            printf("Enter the username correctly\n") ; 
                            send(newsockfd, msg, 4, 0) ;
                            continue ;   
                        }
                    }
                }

                else if(op==2){                                 // pass
                    if(idx == 2){
                        char inp[200];
                        int j =0;
                        for(int i = 5;i<200;i++){
                            if(msg[i]=='\n' || msg[i]=='\0'){
                                inp[j] = '\0';
                                break;
                            }
                            inp[j] =msg[i];
                            j++;
                        }
                        ret_value  = pass(username,inp);
                        if(ret_value){
                            sprintf(msg,"200");
                            op++ ; 
                            send(newsockfd, msg, 4, 0) ;
                        }
                        else{
                            sprintf(msg,"500");  
                            printf("Enter the password correctly\n") ;
                            send(newsockfd, msg, 4, 0) ;
                            continue ;  
                        }
                    }
                    else{
                        printf("Enter the password command\n") ;
                        send(newsockfd, msg, 4, 0) ;
                        continue ;  
                    }
                }

                else{
                    op++ ; 
                    if(idx==3){ //cd
                        char inp[200];
                        int j =0;
                        for(int i = 3;i<200;i++){
                            if(msg[i]=='\n' || msg[i]=='\0'){
                                inp[j] = '\0';
                                break;
                            }
                            inp[j] =msg[i];
                            j++;
                        }

                        char cwd[101];
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("Current working dir: %s\n", cwd);
                        } else {
                            perror("getcwd() error");
                            return 1;
                        }
                        ret_value  = change_dir(inp);
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("Current working dir: %s\n", cwd);
                        } else {
                            perror("getcwd() error");
                            return 1;
                        }
                        if(ret_value){
                            sprintf(msg,"200");
                            send(newsockfd, msg, 4, 0) ;
                            continue  ; 
                        }
                        else{
                            sprintf(msg,"500");
                            send(newsockfd, msg, 4, 0) ;
                            continue ;   
                        }
                    }
                    else if(idx==4){//dir
                        DIR *d;
                        struct dirent *dir;
                        int k ; 
                        d = opendir(".");
                        if (d) {
                            while ((dir = readdir(d)) != NULL) {
                                strcpy(msg,dir->d_name);
                                printf("%s\n", msg);
                                send(newsockfd,msg,sizeof(msg),0);
                                k  = recv(newsockfd,msg,MAXSZ,0);
                                msg[k] = '\0' ;
                            }
                            closedir(d);
                            // bzero(msg,sizeof(msg));
                            sprintf(msg, "200") ; 
                            send(newsockfd, msg, 4, 0);
                        }
                        else{
                            // bzero(msg,sizeof(msg));
                            sprintf(msg, "500") ; 
                            send(newsockfd, msg, 4, 0);
                        } 
                        continue ; 
                    }
                    else if(idx==5){ // get
                        
                        int i = 0 , sp = 0 , j = 0; 
                        char remote_file[200] ; 
                        while(i < n){
                            if(isspace(msg[i])){
                                sp++ ; 
                            }
                            else{
                                if(sp == 1){
                                    remote_file[j++] = msg[i] ; 
                                }
                                else if(sp == 2){
                                    break ; 
                                }
                            }
                            i++ ; 
                        }
                        remote_file[j] = '\0' ; 
                        int rfd = open(remote_file, O_RDONLY ) ; 
                        // printf("The remote file is %s\n", remote_file) ; 
                        char dummy[10] ; 
                        if(rfd < 0){
                            sprintf(msg, "500") ;
                            send(newsockfd,msg,n,0);
                            continue ; 
                        }
                        else{
                            sprintf(msg, "200") ;
                            send(newsockfd,msg,n,0);

                            char fileContents[BLOCK_SIZE+1] ; 
                            bzero(fileContents, sizeof(fileContents)) ; 
                            int sz = read(rfd, fileContents, BLOCK_SIZE) ; 
                            while(sz == BLOCK_SIZE){
                                char tp[2] ; 
                                sprintf(tp, "M") ; 
                                tp[1] = '\0' ; 
                                recv(newsockfd, dummy, 10, 0) ; 
                                send(newsockfd, tp, 2, 0) ; 
                                uint16_t temp = htons(sz) ; 
                                send(newsockfd, &temp, sizeof(uint16_t), 0) ;
                                recv(newsockfd, dummy, 10, 0) ;
                                fileContents[sz] = '\0' ; 
                                recv(newsockfd, dummy, 10, 0) ;
                                send(newsockfd, fileContents, BLOCK_SIZE, 0) ;

                                bzero(fileContents,sizeof(fileContents)) ;  
                                sz = read(rfd, fileContents, BLOCK_SIZE) ;
                            }
                            char tp[2] ; 
                            sprintf(tp, "L") ; 
                            tp[1] = '\0' ;
                            recv(newsockfd, dummy, 10, 0) ;
                            send(newsockfd, tp, 2, 0) ; 
                            fileContents[sz] = '\0' ;
                            recv(newsockfd, dummy, 10, 0) ;
                            send(newsockfd, fileContents, sz, 0) ; 
                            
                            close(rfd) ; 
                            printf("File sent successfully\n") ; 
                            continue ;
                        }
                    }

                    else if(idx==6){ // put 
                        
                        int i = 0 , sp = 0 , j = 0; 
                        char remote_file[200] ; 
                        while(i < n){
                            if(isspace(msg[i])){
                                sp++ ; 
                            }
                            else{
                                if(sp == 2){
                                    remote_file[j++] = msg[i] ; 
                                }
                            }
                            i++ ; 
                        }
                        remote_file[j] = '\0' ; 

                        int rfd = open(remote_file, O_WRONLY | O_CREAT | O_TRUNC, 0644 ) ; 
                        char dummy[10] = "this is" ; 

                        if(rfd < 0){
                            sprintf(msg, "500") ;
                            send(newsockfd,msg,n,0); 
                            continue ;
                        }
                        else{
                            sprintf(msg, "200") ;
                            send(newsockfd,msg,n,0);

                            char headML[2] ; 
                            bzero(headML, sizeof(headML)) ; 
                            int v = recv(newsockfd, headML, 2, 0) ; 
                            headML[v] = '\0' ; 
                            
                            while(headML[0] == 'M'){
                                int sz = BLOCK_SIZE ; 
                                uint16_t szi ; 
                                send(newsockfd, dummy, 10, 0) ; 

                                char recvContent[sz+1];
                                bzero(recvContent,sizeof(recvContent));
                                v = recv(newsockfd,recvContent,sz,0);
                                recvContent[v] = '\0';
                                int siz  = write(rfd,recvContent,v);
                                send(newsockfd,dummy,10,0);
                                bzero(headML,sizeof(headML));

                                recv(newsockfd,headML,2,0);

                            }
                            send(newsockfd,dummy,10,0);
                            int sz = BLOCK_SIZE ; 
                            char recvContent[sz+1];
                            bzero(recvContent,sizeof(recvContent));
                            send(newsockfd,dummy,10,0);
                            v = recv(newsockfd,recvContent,sz,0);
                            recvContent[v] = '\0';
                            int siz  = write(rfd,recvContent,v);
                            printf("Put\n") ; 
                            close(rfd) ; 
                            continue ; 
                        }

                    }

                    else if(idx==7){ /// mget
                        
                        int i = 0 , sp = 0 , j = 0 ;
                        char remote_file[200] ; 
                        bzero(remote_file, sizeof(remote_file)) ; 
                        int sif = 0 ; 
    
                        while((sif = recv(newsockfd, remote_file, 200, 0)) > 0){
                            
                            remote_file[sif] = '\0' ; 
                            int rfd = open(remote_file, O_RDONLY ) ; 

                            char dummy[10] ; 
                            if(rfd < 0){
                                sprintf(msg, "500") ;
                                send(newsockfd,msg,n,0);
                                printf("file error\n")  ; 
                                break ; 
                            }

                            else{
                                sprintf(msg, "200") ;
                                send(newsockfd,msg,n,0);

                                char fileContents[BLOCK_SIZE+1] ; 
                                bzero(fileContents, sizeof(fileContents)) ; 
                                int sz = read(rfd, fileContents, BLOCK_SIZE) ; 
                                while(sz == BLOCK_SIZE){
                                    char tp[2] ; 
                                    sprintf(tp, "M") ; 
                                    tp[1] = '\0' ; 
                                    recv(newsockfd, dummy, 10, 0) ; 
                                    send(newsockfd, tp, 2, 0) ; 
                                    uint16_t temp = htons(sz) ; 
                                    recv(newsockfd, dummy, 10, 0) ;
                                    send(newsockfd, &temp, sizeof(uint16_t), 0) ;
                                    fileContents[sz] = '\0' ; 
                                    recv(newsockfd, dummy, 10, 0) ;
                                    send(newsockfd, fileContents, BLOCK_SIZE, 0) ;

                                    bzero(fileContents,sizeof(fileContents)) ;  
                                    sz = read(rfd, fileContents, BLOCK_SIZE) ;
                                }
                                char tp[2] ; 
                                sprintf(tp, "L") ; 
                                tp[1] = '\0' ;
                                recv(newsockfd, dummy, 10, 0) ;
                                send(newsockfd, tp, 2, 0) ; 
                                fileContents[sz] = '\0' ;
                                recv(newsockfd, dummy, 10, 0) ;
                                send(newsockfd, fileContents, sz, 0) ; 
                                
                                close(rfd) ; 
                                printf("File sent successfully\n") ;
                                bzero(remote_file, sizeof(remote_file)) ;   
                                continue ;
                            }
                        }
                        continue ;  

                    }
                    
                    else if(idx==8){ // mput
                        // printf("%d\n", idx) ; 
                        int i = 0 , sp = 0 , j = 0 ;
                        char remote_file[200] ; 
                        bzero(remote_file, sizeof(remote_file)) ; 
                        int sif = 0 ; 
    
                        while((sif = recv(newsockfd, remote_file, 200, 0)) > 0){
                            
                            remote_file[sif] = '\0' ; 
                            int rfd = open(remote_file,O_WRONLY | O_CREAT | O_TRUNC, 0644 ) ; 

                            char dummy[10] = "this is" ; 
                            if(rfd < 0){
                                sprintf(msg, "500") ;
                                send(newsockfd,msg,n,0);
                                printf("file error\n")  ; 
                                break ; 
                            }
                            sprintf(msg, "200") ;
                            send(newsockfd,msg,n,0);

                            char headML[2] ; 
                            bzero(headML, sizeof(headML)) ; 
                            int v = recv(newsockfd, headML, 2, 0) ; 
                            headML[v] = '\0' ; 
                            
                            while(headML[0] == 'M'){
                                int sz = BLOCK_SIZE ; 
                                uint16_t szi ; 
                                send(newsockfd, dummy, 10, 0) ; 

                                char recvContent[sz+1];
                                bzero(recvContent,sizeof(recvContent));
                                v = recv(newsockfd,recvContent,sz,0);
                                recvContent[v] = '\0';
                                int siz  = write(rfd,recvContent,v);
                                send(newsockfd,dummy,10,0);
                                bzero(headML,sizeof(headML));

                                recv(newsockfd,headML,2,0);

                            }
                            send(newsockfd,dummy,10,0);
                            int sz = BLOCK_SIZE ; 
                            char recvContent[sz+1];
                            bzero(recvContent,sizeof(recvContent));
                            
                            v = recv(newsockfd,recvContent,sz,0);
                            send(newsockfd,dummy,10,0);
                            recvContent[v] = '\0';
                            int siz  = write(rfd,recvContent,v);
                            printf("File put successfully\n") ; 
                            close(rfd) ; 
                            bzero(remote_file, sizeof(remote_file)) ;   
                            continue ; 
                        }
                        continue ;  

                    }

                    else{
                        sprintf(msg, "500") ; 
                        send(newsockfd, msg, 4, 0) ;
                        continue ; 
                    }

                }
            }
            
            //close interior while
            exit(0);
        }
        
        else{
            close(newsockfd);//sock is closed BY PARENT
        }
    }//close exterior while

    return 0;
}