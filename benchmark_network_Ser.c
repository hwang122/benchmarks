#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 5000000


void *Ser_TCP(void *thread_id)
{
	int ser_sockfd, cli_sockfd;
	int err, size;
	struct sockaddr_in ser_addr;
	char *buffer;
	socklen_t addrlen;
	int tid = (int)(long)thread_id;

	ser_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(ser_sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}

	//set TCP packet size
	//setsockopt(ser_sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&RecvBuf, sizeof(int));

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(PORT + tid);
	err = bind(ser_sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
	if(err == -1)
	{
		printf("Error: bind\n");
		exit(-1);
	}

	err = listen(ser_sockfd, 3);
	if(err == -1)
	{
		printf("Error: listen\n");
		exit(-1);
	}

	printf("Start listening...\n");

	//can be connect for 10 times
	while(1)
	{
		addrlen = sizeof(struct sockaddr);
		cli_sockfd = accept(ser_sockfd, (struct sockaddr *)&cli_sockfd, &addrlen);
		if(cli_sockfd == -1)
		{
			printf("Error: accept\n");
			exit(-1);
		}

		buffer = (char*)malloc(BUFFER_SIZE);
		size = BUFFER_SIZE;
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

	close(ser_sockfd);
}

void *Ser_UDP(void *thread_id)
{
	int sockfd;
	int err, size;
	struct sockaddr_in addr_ser, addr_cli;
	char *buffer;
	socklen_t addrlen;

	int tid = (int)(long)thread_id;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}

	//set UDP packet size
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&RecvBuf, sizeof(int));

	memset(&addr_ser, 0 ,sizeof(addr_ser));
	addr_ser.sin_family = AF_INET;
	addr_ser.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_ser.sin_port = htons(PORT + tid);

	err = bind(sockfd, (struct sockaddr *)&addr_ser, sizeof(addr_ser));
	if(err == -1)
	{
		printf("Error: bind\n");
		exit(-1);
	}

	addrlen = sizeof(struct sockaddr);

	printf("Start listening...\n");

	while(1)
	{
		buffer = (char*)malloc(BUFFER_SIZE);
		memset(buffer, 0, BUFFER_SIZE);

		size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,\
		 (struct sockaddr *)&addr_cli, &addrlen);
		if(size == -1)
		{
			printf("Error: recvfrom\n");
			exit(-1);
		}
	}

	close(sockfd);
}

int main(int argc, char const *argv[])
{
	if(argc != 3)
	{
		printf("usage: %s <connect type> <num of thread>\n", argv[0]);
		printf("connect type: \n-t    TCP\n-u    UDP\n");
		exit(-1);
	}

	int i;
	int numThread = atoi(argv[2]);
	pthread_t tid[numThread];

	if(strcmp(argv[1], "-t") == 0)
	{
		//TCP
		for (i = 0; i < numThread; i++)
		{
			pthread_create(&tid[i], NULL, Ser_TCP, (void*)(long)i);
		}
	}
	else if(strcmp(argv[1], "-u") == 0)
	{
		//UDP
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
