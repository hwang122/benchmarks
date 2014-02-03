#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

//port number
#define PORT 8888
//buffer size to be received
#define BUFFER_SIZE 5000000

//server TCP
void *Ser_TCP(void *thread_id)
{
	int ser_sockfd, cli_sockfd;
	int err, size;
	struct sockaddr_in ser_addr;
	char *buffer;
	socklen_t addrlen;
	int tid = (int)(long)thread_id;
	
	//create socket
	ser_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ser_sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}
	
	//set server address and port
	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//may use multi ports at the same time
	ser_addr.sin_port = htons(PORT + tid);
	//bind port to the socket
	err = bind(ser_sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(err == -1)
	{
		printf("Error: bind\n");
		exit(-1);
	}
	
	//start listening the port
	err = listen(ser_sockfd, 3);
	if(err == -1)
	{
		printf("Error: listen\n");
		exit(-1);
	}

	printf("Start listening...\n");

	//receive message from the client
	while(1)
	{
		addrlen = sizeof(struct sockaddr);
		//connected with the client
		cli_sockfd = accept(ser_sockfd, (struct sockaddr *)&cli_sockfd, &addrlen);
		if(cli_sockfd == -1)
		{
			printf("Error: accept\n");
			exit(-1);
		}

		buffer = (char*)malloc(BUFFER_SIZE);
		size = BUFFER_SIZE;
		//receive messages from client
		//if size is 0, all messages have been sent
		while(size > 0)
		{
			size = recv(cli_sockfd, buffer, BUFFER_SIZE, 0);
			if(size == -1)
			{
				printf("Error: recv\n");
				exit(-1);
			}
		}
		close(cli_sockfd);
	}
	//close socket
	close(ser_sockfd);
}

//server UDP
void *Ser_UDP(void *thread_id)
{
	int sockfd;
	int err, size;
	struct sockaddr_in addr_ser, addr_cli;
	char *buffer;
	socklen_t addrlen;

	int tid = (int)(long)thread_id;
	//create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}
	
	//set server address and port
	memset(&addr_ser, 0 ,sizeof(addr_ser));
	addr_ser.sin_family = AF_INET;
	addr_ser.sin_addr.s_addr = htonl(INADDR_ANY);
	//multi ports may be used
	addr_ser.sin_port = htons(PORT + tid);
	
	//bind port to the socket
	err = bind(sockfd, (struct sockaddr *)&addr_ser, sizeof(addr_ser));
	if(err == -1)
	{
		printf("Error: bind\n");
		exit(-1);
	}

	addrlen = sizeof(struct sockaddr);

	printf("Start receiving...\n");
	//receive datagram from the client, there is no need to connect
	while(1)
	{
		buffer = (char*)malloc(BUFFER_SIZE);
		memset(buffer, 0, BUFFER_SIZE);
		
		//receive messages from client
		size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,\
		 (struct sockaddr *)&addr_cli, &addrlen);
		if(size == -1)
		{
			printf("Error: recvfrom\n");
			exit(-1);
		}
	}
	//close socket
	close(sockfd);
}

int main(int argc, char const *argv[])
{
	//check input parameters
	if(argc != 3)
	{
		printf("usage: %s <connect type> <num of thread>\n", argv[0]);
		printf("connect type: \n-t    TCP\n-u    UDP\n");
		exit(-1);
	}

	int i;
	//number of threads
	int numThread = atoi(argv[2]);
	pthread_t tid[numThread];
	
	//connect type is TCP
	if(strcmp(argv[1], "-t") == 0)
	{
		//run several servers at the same time
		for (i = 0; i < numThread; i++)
		{
			pthread_create(&tid[i], NULL, Ser_TCP, (void*)(long)i);
		}
	}
	//connect type is UDP
	else if(strcmp(argv[1], "-u") == 0)
	{
		//run several servers at the same time
		for(i = 0; i < numThread; i++)
		{
			pthread_create(&tid[i], NULL, Ser_UDP, (void*)(long)i);
		}
	}
	else
	{
		printf("Error connect type\n");
		exit(-1);
	}

	//wait until all the threads end
	for (i = 0; i < numThread; i++)
	{
		pthread_join(tid[i], NULL);
	}

	return 0;
}
