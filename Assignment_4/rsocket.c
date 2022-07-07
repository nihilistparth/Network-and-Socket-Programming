/*
Parth Tusham
Shashwat Shukla
Group 23
*/
#include "rsocket.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <netinet/in.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

/********************************************************************/

// Message format
// Data :           <msg> id 
// Ack :            ACK id

int dropMessage(){
    float r = (float)rand() / ((float)RAND_MAX) ; 
    return r < p ; 
}

/********************************************************************/

pthread_mutex_t lock ; 
pthread_t tidR, tidS ; 
int udp_fd = -1 ; 
int tot_trans = 0 ; 
int msg_id = 0 ; 
struct sockaddr_in source_addr_recv ;
socklen_t addr_len_recv = 0 ;  
int recv_flags = 0 ;


/********************************************************************/

typedef struct unack{
    int id ; 
    char msg[MAX_MSG_SIZE] ; 
    size_t msg_len ; 
    time_t tim ; 
    int flags ; 
    struct sockaddr_in destination ; 
    socklen_t addrlen ; 
    pthread_mutex_t lock ; 
} unACK ; 

typedef struct recv_msg{
    int id ; 
    char msg[MAX_MSG_SIZE] ; 
    struct sockaddr_in source ; 
    socklen_t addrlen ; 
} recvMsg ; 

unACK* unacknowledged_message_table ; 
recvMsg* recieve_message_table ; 

/********************************************************************/

void printUnack(unACK obj) ; 

void initialize_tables() ; 
int getEmptyIndex_unACK() ; 
int getEmptyIndex_recvMsg() ; 
size_t produceFinalMessage(int id, char* buf, int len) ; 

int decodeRecvMessage(char* msg, int *id) ; 

int getNumber(char* msg, int from) ; 

void HandleData() ; 
ssize_t sendACK(int id, struct sockaddr_in addr, socklen_t addr_len) ; 
void UpdateACK(int id) ; 
int UpdateRecvMsg(int id, char *buf, struct sockaddr_in source_addr, socklen_t addr_len) ; 

/********************************************************************/

// Socket Call
int r_socket(int domain, int type, int protocol){

    if(type != SOCK_MRP)
        return -1 ; 

    if ((udp_fd = socket(domain, SOCK_DGRAM, protocol)) < 0)
        return udp_fd;
    
    unacknowledged_message_table = (unACK*)malloc(MAX_TABLE_SIZE * sizeof(unACK)) ; 
    recieve_message_table = (recvMsg*)malloc(MAX_TABLE_SIZE*sizeof(recvMsg)); 

    initialize_tables() ; 

    pthread_mutex_init(&lock, NULL) ; 

    pthread_attr_t attrR, attrS ; 
    pthread_attr_init(&attrR) ; 
    pthread_attr_init(&attrS) ;

    int retR = pthread_create(&tidR, &attrR, runnerR, NULL) ; 
    int retS = pthread_create(&tidS, &attrS, runnerS, NULL) ;

    if(retR < 0 || retS < 0)
        return -1 ; 

    return udp_fd ; 
}

// Bind call
int r_bind(int socket, const struct sockaddr *address, socklen_t address_len){
    return bind(socket, address, address_len) ; 
}

// Send Call
ssize_t r_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen){

    if (sockfd != udp_fd)
        return -1;

    // printf("yes 1\n") ; 

    int idx = getEmptyIndex_unACK() ; 
    if(idx == -1){
        return -1; 
    }
    // printf("yes 2\n") ; 

    char buff[MAX_MSG_SIZE] ; 
    strcpy(buff, (char* )buf) ; 

    for (int i = 1 ; i < strlen(buff) ; i++){
        buff[i] = '\0' ; 
    }

    // printf("yes 3\n") ; 
    pthread_mutex_lock(&(lock)) ; 
    // pthread_mutex_lock(&(unacknowledged_message_table[idx].lock)) ; 
    
    // printf("yes 4\n") ; 

    unacknowledged_message_table[idx].id = msg_id ; 


    unacknowledged_message_table[idx].tim = time(NULL) ; 

    sprintf(unacknowledged_message_table[idx].msg, "%c%d", buff[0], msg_id); 

    unacknowledged_message_table[idx].destination = *(struct sockaddr_in *)dest_addr ; 
    unacknowledged_message_table[idx].addrlen = addrlen ;  

    size_t finalbyte = sizeof(unacknowledged_message_table[idx].msg) ;  

    unacknowledged_message_table[idx].msg_len = finalbyte ;

    pthread_mutex_unlock(&lock) ; 

    ssize_t r = sendto(sockfd, unacknowledged_message_table[idx].msg, 
                        unacknowledged_message_table[idx].msg_len, 
                        unacknowledged_message_table[idx].flags, 
                        (struct sockaddr*)&unacknowledged_message_table[idx].destination, 
                        unacknowledged_message_table[idx].addrlen) ;     

    msg_id++ ; 
    tot_trans++ ;
    printf("%d\n", tot_trans) ;
    // printf("yes 7\n") ;  
    return r ; 
}

ssize_t r_recvfrom(int sockfd, char *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){

    if (sockfd != udp_fd)
        return -1;

    while(1){
        // printf("yes 1.1\n") ; 
        int i = getEmptyIndex_recvMsg() ; 
        if (i != -1){
            // printf("yes 1.2 %d\n", i) ;

            strcpy(buf, recieve_message_table[i].msg);  
            recieve_message_table[i].source = source_addr_recv ;

            if (len >= 0 && len < strlen(buf)){
                buf[len] = '\0' ; 
            }

            // printf("yes 1.3\n") ;
            len = strlen(buf) ; 
            *src_addr = *(struct sockaddr*)&source_addr_recv ; 
            *addrlen = addr_len_recv ; 
            recv_flags = flags ; 
            
            // printf("Msg recieved %s\n", buf) ; 

            // this implies now this recieve message has been deleted
            recieve_message_table[i].id = -1 ; 
            return len ; 
        }
        else{
            usleep(100) ; 
        }
    }   
}

int r_close(int fd){

    if (fd != udp_fd){
        return -1 ; 
    }

    while(1){

        int flag = 0; 
        for(int i = 0 ; i < MAX_TABLE_SIZE ; i++){
            if(unacknowledged_message_table[i].id != -1)
                flag = 1 ; 
        }
        if(!flag){
            break ; 
        }
    }
    
    printf("No of transmissions = %d\n", msg_id);
    pthread_kill(tidR, SIGKILL) ;
    pthread_kill(tidS, SIGKILL) ; 

    for (int i = 0; i < MAX_TABLE_SIZE; i++){
        free(unacknowledged_message_table[i].msg);
        free(recieve_message_table[i].msg) ; 
    }
    free(unacknowledged_message_table);
    free(recieve_message_table);

    return close(fd);
    
}

void* runnerR(void* param){

    fd_set rfds ; 

    while(1){
        // printf("yes 2.1\n") ;
        FD_ZERO(&rfds) ; 
        FD_SET(udp_fd, &rfds) ; 
        // printf("yes 2.2\n") ;
        int r = select(udp_fd + 1, &rfds, NULL, NULL, NULL);

        if (r < 0){
            perror("Select Failed\n");
        }
        else if (r > 0){
            // printf("yes 2.3\n") ;
            if (FD_ISSET(udp_fd, &rfds)){
                // printf("yes 2.4\n") ;
                HandleData() ; 
                // printf("The total number of transmissions till now %d\n", tot_trans) ; 
            }
        }
    }

}

void* runnerS(void* param){

    while(1){

        sleep(T) ; 
        int idx = -1 ;
        pthread_mutex_lock(&lock) ; 
        for(int i = 0 ; i < MAX_TABLE_SIZE ; i++){
            // pthread_mutex_lock(&(unacknowledged_message_table[i].lock)) ; 
            if(unacknowledged_message_table[i].id != -1){
                time_t time_now = time(NULL);
                if(time_now - unacknowledged_message_table[i].tim > 2*T){
                    // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ;

                    idx = i ; 
                    ssize_t r = sendto(udp_fd, unacknowledged_message_table[idx].msg, 
                                    unacknowledged_message_table[idx].msg_len, 
                                    unacknowledged_message_table[idx].flags, 
                                    (struct sockaddr*)&unacknowledged_message_table[idx].destination, 
                                    unacknowledged_message_table[idx].addrlen) ;     
                    unacknowledged_message_table[idx].tim = time_now ; 
                    tot_trans++ ;
                    printf("Sent unack message again\n") ;  
                    printf("%d\n", tot_trans) ;
                    // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ;       
                    break ;
                }
            }
            // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ;
        }
        pthread_mutex_unlock(&lock) ; 
        // pthread_mutex_lock(&(unacknowledged_message_table[idx].lock)) ;  
    }

}

/********************************************************************/

void initialize_tables(){
    int i ; 
    for(i = 0 ; i < MAX_TABLE_SIZE ; i++){
        recieve_message_table[i].id = -1 ;
        unacknowledged_message_table[i].id = -1 ;
        pthread_mutex_init(&(unacknowledged_message_table[i].lock), NULL) ; 
    }

}

int getEmptyIndex_unACK(){
    int i ; 
    pthread_mutex_lock(&lock) ;
    for(i = 0 ; i < MAX_TABLE_SIZE ; i++){
        // pthread_mutex_lock(&(unacknowledged_message_table[i].lock)) ; 
        if(unacknowledged_message_table[i].id == -1){
            // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ;
            pthread_mutex_unlock(&lock) ;
            return i ; 
        }
        // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ;
    }
    pthread_mutex_unlock(&lock) ;
    return -1 ; 
}   

size_t produceFinalMessage(int id, char* buf, int len){
    return len + sizeof(id);
}

int getEmptyIndex_recvMsg(){
    int i ; 
    for(i = 0 ; i < MAX_TABLE_SIZE ; i++){  
        if(recieve_message_table[i].id != -1){
            return i ; 
        }
    }
    return -1 ; 
}

void HandleData(){

    char buf[MAX_MSG_SIZE] ; 
    bzero(buf, sizeof(buf)) ; 

    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    int n = recvfrom(udp_fd, buf, MAX_MSG_SIZE, recv_flags, (struct sockaddr *)&source_addr, &addr_len);
    // printf("the size is %d\n", (int)strlen(buf)) ; 
    int l = strlen(buf) ; 
    buf[l] = '\0';

    if (dropMessage()){
        printf("case\n") ; 
        return ; 
    }
    // ACK<>
    // a<>
    printf("msg recieved %s\n", buf) ; 

    int id ; 
    int check = decodeRecvMessage(buf, &id) ; 

    printf("%d, %d\n", check, id) ; 

    if(check == 0){
        // ACK message
        UpdateACK(id) ; 
    }
    else{
        // data message
        UpdateRecvMsg(id, buf, source_addr, addr_len) ; 
    }

}

void UpdateACK(int id){
    int i = 0 ; 
    printf("id is %d\n", id) ; 
    pthread_mutex_lock(&lock) ; 
    for(i = 0 ; i < MAX_TABLE_SIZE ; i++){
        if(unacknowledged_message_table[i].id != id)
            continue ; 
        
        // pthread_mutex_lock(&(unacknowledged_message_table[i].lock)) ; 
        printf("ACK %d\n", id) ; 
        unacknowledged_message_table[i].id = -1 ; 
        // pthread_mutex_unlock(&(unacknowledged_message_table[i].lock)) ; 
        break ; 
    }
    // printf("wrong ACK %d\n", id) ; 
    pthread_mutex_unlock(&lock) ; 
}

int decodeRecvMessage(char* msg, int *id){

    printf("message is : %s %d\n", msg, (int)strlen(msg)) ; 

    int ret  = 0 ; 
    int l = 3 ; 

    if(l >= strlen(msg)){
        l = strlen(msg) ; 
    }

    if(l >= 3){
        if(msg[0] == 'A' && msg[1] == 'C' && msg[2] == 'K'){
            ret = getNumber(msg, 3) ;  
            *id = ret ; 
            return 0 ; 
        }
        else{
            ret = getNumber(msg, 1) ; 
            *id = ret ;
            return 1 ; 
        }
    }
    else{
        ret = getNumber(msg, 1) ;
        
        // printf("ret value 3 %d\n", ret) ;
        *id = ret ;
        return 1 ; 
    }
}

ssize_t sendACK(int id, struct sockaddr_in addr, socklen_t addr_len){

    char ACK[MAX_MSG_SIZE] ; 
    bzero(ACK, sizeof(ACK)) ; 
    sprintf(ACK, "ACK%d", id);
    size_t ret = sizeof(ACK) ; 
    // size_t ret = produceFinalMessage(id, ACK, -1);
    size_t r = sendto(udp_fd, ACK, ret, 0, (struct sockaddr *)&addr, addr_len);
    return r ; 
}

int UpdateRecvMsg(int id, char *buf, struct sockaddr_in source_addr, socklen_t addr_len){

    for(int i = 0 ; i < MAX_TABLE_SIZE ; i++){
        if(recieve_message_table[i].id == id){
            
            source_addr_recv = source_addr;
            addr_len_recv = addr_len ; 
            
            sendACK(id, source_addr, addr_len) ; 
            return 0 ; 
        }
    }

    // int i = getEmptyIndex_recvMsg()
    int flag = 0 ; 
    for(int i = 0 ; i < MAX_TABLE_SIZE ; i++){
        if(recieve_message_table[i].id == -1){
            source_addr_recv = source_addr;
            addr_len_recv = addr_len ; 

            recieve_message_table[i].id = id ; 
            recieve_message_table[i].source = source_addr ; 
            recieve_message_table[i].addrlen = addr_len ; 
            sprintf(recieve_message_table[i].msg, "%s", buf) ; 
            sendACK(id, source_addr, addr_len) ; 
            return 0 ; 
        }
    }

    return -1 ; 
}

void printUnack(unACK obj){

    printf("Id is %d\n", obj.id) ; 
    printf("msg is %s\n", obj.msg) ; 

}

int getNumber(char* msg, int from){
    int ret = 0 ; 

    for(int i = from ; i < strlen(msg) ; i++){
        ret *= 10 ; 
        char c = msg[i] ; 
        ret += (c - '0') ; 
    }
    return ret ; 
}