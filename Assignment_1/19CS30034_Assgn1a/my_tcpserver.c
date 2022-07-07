// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#define PORT 8080
#define MAXLEN 100
int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[100] = {0};
	char *hello = "Hello from server";
	
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	printf("************************************\n");
	printf("Weclome to Server \n");
	printf("************************************\n");
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	while(1){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}	
		int sentences =0,chars =0,words =0;
		int cur_char =0;
		char* recvmsg = "RECIEVED";
		while(1){
			valread = recv(new_socket,buffer,MAXLEN,0);
		
			buffer[valread] = '\0';
			printf("RECIEVING IN PROGRESS ..... \n");
			chars+=valread;
		
			if(valread>0){
				int i =0;
				// counting characters, words and sentences
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
					else if((c<='z' && c>='a') || (c<='Z' && c>='A')|| (c >= '0' && c <= '9')){
						cur_char++;
					}
				}
				// returning statistics back to client 
				char msg[100];
				sprintf(msg,"characters are %d ,words are %d and sentences are %d",chars,words,sentences);
				send(new_socket,msg,strlen(msg),0);
			}
			else{
				printf("RECIEVING COMPLETED !\n");
				close(new_socket);
				break;
			}
		}
	}
	
	return 0;
}