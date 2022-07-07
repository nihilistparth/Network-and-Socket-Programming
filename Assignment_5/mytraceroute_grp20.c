// Name         :       Shashwat Shukla
//              :       Parth Tusham
// Group 20     

// option 1 chose Traceroute using Raw sockets

/* Running Instruction
gcc mytraceroute_grp20.c -o trace
sudo ./trace www.iitkgp.ac.in 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define N 52
#define MSG_SIZE 2048
#define MAX_CHAR 100
#define PCKT_LEN 8192
#define TIMEOUT 1
#define MAX_HOP 30
#define DEST_PORT 32164
#define S_PORT 8080

/************************************************************************************/

void setIPHDR(struct iphdr* ip){
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0; // low delay
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + N; //https://tools.ietf.org/html/rfc791#page-11
    ip->id = htons(54322);
    ip->protocol = 17; // UDP
    ip->saddr = 0;     //src_addr;
}

void setUDPhdr(struct udphdr* udp, uint16_t src_port, uint16_t dst_port){
    udp->source = htons(src_port);
    udp->dest = htons(dst_port);
    udp->len = htons(sizeof(struct udphdr)+N);
}

void timeoutFunc(int* times, int* ttl, int* timeout, int* is_send, int flag){
    if(flag == 0){
        (*times) = 1 ; 
        (*ttl)++ ; 
    }
    else if(flag == -1){
        (*timeout) = TIMEOUT ;
        (*is_send) = 1 ;
        return ;     
    }
    else{
        if (*times > 3){
            printf("%d\t*\t*\n", *ttl);
            (*times) = 1 ; 
            (*ttl)++ ; 
        }
    }
    (*timeout) = TIMEOUT ;
    (*is_send) = 1 ;  
}

void close_and_exit(int* rawfd1, int* rawfd2){
    close(*rawfd1) ;
    close(*rawfd2) ; 
}

void printStats(int ttl, struct in_addr saddr_ip, float end_time, float start_time){
    printf("%d\t", ttl);
    printf("%s\t", inet_ntoa(saddr_ip));
    printf("%.3f ms\n", (float)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
}

void addTochecksum(unsigned long* sum){
    (*sum) = ((*sum) >> 16) + ((*sum) & 0xffff);
    (*sum) += ((*sum) >> 16);
}

unsigned short getCsum(unsigned short *buf, int nwords){
    unsigned long sum = 0;
    while(nwords--)
        sum += *buf++;
    addTochecksum(&sum) ;
    return (unsigned short)(~sum) ;
}

/************************************************************************************/
int main(int argc, char *argv[]){
    int rawfd1, rawfd2;
    struct sockaddr_in saddr_raw, cli_addr;
    socklen_t saddr_raw_len;

    if (argc != 2){
        printf("Error: Invalid parameters!\n");
        printf("Usage: %s <target hostname/IP>\n", argv[0]);
        exit(1);
    }

    char ipaddr[MAX_CHAR];    
    struct hostent *he;
    struct in_addr **addr_list;
    
    if ((he = gethostbyname(argv[1])) == NULL){
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **)he->h_addr_list;
    if (addr_list[0] == NULL){
        herror("gethostbyname");
        return 1;
    }
    
    strcpy(ipaddr, inet_ntoa(*addr_list[0]));
    
    if ((rawfd1 = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0){
        perror("Socket error");
        exit(1);
    }
    if ((rawfd2 = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0){
        perror("Socket error");
        exit(1);
    }
    
    u_int16_t src_port, dst_port;
    u_int32_t dst_addr;
    dst_addr = inet_addr(ipaddr);
    src_port = S_PORT;
    dst_port = DEST_PORT;
    saddr_raw.sin_family = AF_INET;
    saddr_raw.sin_port = htons(src_port);
    saddr_raw.sin_addr.s_addr = INADDR_ANY;
    saddr_raw_len = sizeof(saddr_raw);
    
    int one = 1;
    const int *val = &one;
    
    if (bind(rawfd1, (struct sockaddr *)&saddr_raw, saddr_raw_len) < 0){
        perror("raw bind");
        close_and_exit(&rawfd1, &rawfd2) ; 
        return 1 ; 
    }

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(dst_port);
    cli_addr.sin_addr.s_addr = dst_addr;

    printf("mytraceroute to %s (%s), %d hops max, %d byte packets\n", argv[1], ipaddr, MAX_HOP, N);

    if (setsockopt(rawfd1, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0){
        fprintf(stderr, "Error: setsockopt. You need to run this program as root\n");
        close_and_exit(&rawfd1, &rawfd2) ;
        return 1 ; 
    }
    
    int ttl = 1, timeout = TIMEOUT, is_send = 1;
    fd_set readSockSet;
    int times = 0;
    char payload[52];
    clock_t start_time;

    while (ttl < MAX_HOP){
        char buffer[PCKT_LEN];
        struct iphdr *ip = (struct iphdr *)buffer;
        struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct iphdr));

        FD_ZERO(&readSockSet);
        FD_SET(rawfd2, &readSockSet);
        struct timeval tv = {timeout, 0};

        if (is_send){
            times++;
            int i_temp = 0 ; 
            while(i_temp < N){
                payload[i_temp++] = rand() % 26 + 'A';
            }
            payload[N-1] = '\0';

            bzero(buffer, PCKT_LEN) ; 
            setIPHDR(ip) ; 
            ip->daddr = dst_addr;
            ip->ttl = ttl;     
            
            setUDPhdr(udp, src_port, dst_port) ; 

            unsigned short* buf = (unsigned short *)buffer ; 
            int nwords = sizeof(struct iphdr) + sizeof(struct udphdr) ; 
            ip->check = getCsum(buf, nwords) ;

            strcpy(buffer + sizeof(struct iphdr) + sizeof(struct udphdr), payload);
            if (sendto(rawfd1, buffer, ip->tot_len, 0,(struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0){
                close_and_exit(&rawfd1, &rawfd2) ;
                return 0 ; 
            }
            start_time = clock();
        }
        
        int ret = select(rawfd2 + 1, &readSockSet, 0, 0, &tv);
        char msg[MAX_CHAR];
                
        if (ret == -1){
            close_and_exit(&rawfd1, &rawfd2) ;
            return 0; 
        }
        
        else if (ret > 0){
        
            if (FD_ISSET(rawfd2, &readSockSet)){
        
                bzero(msg, sizeof(msg)) ; 
                socklen_t raddr_len = sizeof(saddr_raw);
                int msglen = recvfrom(rawfd2, msg, MSG_SIZE, 0, (struct sockaddr *)&saddr_raw, &raddr_len);
                clock_t end_time = clock();
                
                if (msglen <= 0){
                    timeoutFunc(&times, &ttl, &timeout, &is_send, -1) ; 
                    continue;
                }
                
                struct iphdr hdrip = *((struct iphdr *)msg);
                int iphdrlen = sizeof(hdrip);
                struct icmphdr hdricmp = *((struct icmphdr *)(msg + iphdrlen));
                
                struct in_addr saddr_ip;
                saddr_ip.s_addr = hdrip.saddr;

                int ck = hdrip.protocol ; 
                if (ck == 1) {
                    int chk = hdricmp.type ; 
                    // This is the chk for the timeout 
                    if (chk == 11){
                        printStats(ttl, saddr_ip, end_time, start_time) ; 
                        timeoutFunc(&times, &ttl, &timeout, &is_send, 0) ; 
                    }
                    // This is to verify
                    else if (chk == 3){
                        if (hdrip.saddr == ip->daddr){
                            printStats(ttl, saddr_ip, end_time, start_time) ;
                        }
                        close_and_exit(&rawfd1, &rawfd2) ;
                        return 0;                        
                    }
                }
                else{
                    is_send = 0;
                    timeout = end_time - start_time;
                    if(timeout < 0.01){
                        timeoutFunc(&times, &ttl, &timeout, &is_send, 1) ; 
                    }
                }
            }
        }
        else{
            timeoutFunc(&times, &ttl, &timeout, &is_send, 1) ;
        }
    }
    close_and_exit(&rawfd1, &rawfd2) ;
    return 0;
} 