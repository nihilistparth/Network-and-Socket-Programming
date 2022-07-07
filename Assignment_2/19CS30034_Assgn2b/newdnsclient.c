
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<fcntl.h> 
#include<errno.h> 
#define PORT 8181
#define MAXLEN 100
#define CHUNKSIZE 50
int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[105] = {0};
	
    printf("Enter the DNS name \n");
    scanf("%s",buffer);
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
	
    printf("SENDING..... \n");
    send(sock,buffer,strlen(buffer),0); // send data to server
    valread = recv(sock,buffer,MAXLEN,0);// recieve data from server
    buffer[valread] = '\0';
	printf("INFORMATION RECIEVED BACK IS ");
	printf("%s \n",buffer);
	close(sock);
	// close(fd);
	
	return 0;
}