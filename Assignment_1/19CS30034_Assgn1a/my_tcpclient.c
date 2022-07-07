
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<fcntl.h> 
#include<errno.h> 
#define PORT 8080
#define MAXLEN 100
#define CHUNKSIZE 50
int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	const char *file_name = argv[1]; // file name taken command line argument
	char buffer[105] = {0};
	int fd = 0; 
	if((fd = open(file_name,O_RDONLY))<0){
		printf("File %s not found ! \n",file_name);
		return -1;
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	// connect to server
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	char* recvmsg = "RECIEVED";
	while((valread = read(fd,buffer,CHUNKSIZE))>0){
		buffer[valread] = '\0';
		printf("SENDING..... \n");
		send(sock,buffer,strlen(buffer),0); // send data to server

		valread = recv(sock,buffer,MAXLEN,0);// recieve data from server
		buffer[valread] = '\0';
		
	}
	printf("File sent \n");
	printf("INFORMATION RECIEVED BACK IS ");
	printf("%s \n",buffer);
	close(sock);
	close(fd);
	
	return 0;
}