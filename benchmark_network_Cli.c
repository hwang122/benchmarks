#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8888
#define BUFFER_SIZE 1000000

typedef struct Data
{
	int packet_size;
	int thread_id;
} data;

void *Cli_TCP(void *arg)
{
	int sockfd, err, i;
	struct sockaddr_in addr_ser;
	char *buffer;

	int SendBuf, tid;
	data *pData;
	pData = (data *)arg;
	SendBuf = pData->packet_size;
	tid = pData->thread_id;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}

	//set TCP packet size
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&SendBuf, sizeof(int));

	memset(&addr_ser, 0, sizeof(addr_ser));
	addr_ser.sin_family = AF_INET;
	addr_ser.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_ser.sin_port = htons(PORT + tid);
	err = connect(sockfd, (struct sockaddr *)&addr_ser, sizeof(addr_ser));
	if(err == -1)
	{
		printf("Error: connect\n");
		exit(-1);
	}

	printf("Start sending...\n");

	for(i = 0; i < 5; i++)
	{
		buffer = (char*)malloc(BUFFER_SIZE);
		memset(buffer, 'a', BUFFER_SIZE);

		send(sockfd, buffer, BUFFER_SIZE, 0);
	}

	close(sockfd);
}

void *Cli_UDP(void *arg)
{
	int sockfd, size, i;
	struct sockaddr_in addr_ser;
	char *buffer;
	socklen_t addrlen;

	int SendBuf, tid;
	data *pData;
	pData = (data *)arg;
	SendBuf = pData->packet_size;
	tid = pData->thread_id;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("Error: socket\n");
		exit(-1);
	}

	memset(&addr_ser, 0, sizeof(addr_ser));
	addr_ser.sin_family = AF_INET;
	addr_ser.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_ser.sin_port = htons(PORT + tid);

	addrlen = sizeof(addr_ser);

	printf("Start sending...\n");

	for(i = 0; i < 5; i++)
	{
		buffer = (char*)malloc(BUFFER_SIZE);
		memset(buffer, 'a', BUFFER_SIZE);
		size = BUFFER_SIZE;
		while(size > 0)
		{
			size = sendto(sockfd, buffer, SendBuf, 0, \
			(struct sockaddr *)&addr_ser, addrlen);
			size -= SendBuf;
			if(size == -1)
			{
				printf("Error: sendto: %s\n", strerror(errno));
				exit(-1);
			}
		}
	}

	close(sockfd);
}

int main(int argc, char const *argv[])
{
	if(argc != 4)
	{
		printf("usage: %s <connect type> <packet size> <num of thread>\n", argv[0]);
		printf("connect type: \n-t    TCP\n-u    UDP\n");
		printf("packet size: 1b, 1kb, 64kb\n");
		exit(-1);
	}

	int SendBuf, i;
	int numThread = atoi(argv[3]);
	pthread_t tid[numThread];

	if(strcmp(argv[2], "1b") == 0)
	{
		SendBuf = 1;
	}
	else if (strcmp(argv[2], "1kb") == 0)
	{
		SendBuf = 1024;
	}
	else if(strcmp(argv[2], "64kb") == 0)
	{
		SendBuf = 65536;
	}
	else
	{
		printf("Error: packet size\n");
		exit(-1);
	}

	data d;
	d.packet_size = SendBuf;

	//variables to count the time
	struct timeval etstart, etstop;
	unsigned long long usecstart, usecstop;

	gettimeofday(&etstart, NULL);

	if(strcmp(argv[1], "-t") == 0)
	{
		//TCP
		for (i = 0; i < numThread; i++)
		{
			d.thread_id = i;
			pthread_create(&tid[i], NULL, Cli_TCP, &d);
		}
	}
	else if(strcmp(argv[1], "-u") == 0)
	{
		//UDP
		for(i = 0; i < numThread; i++)
		{
			d.thread_id = i;
			pthread_create(&tid[i], NULL, Cli_UDP, &d);
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

	gettimeofday(&etstop, NULL);
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

	float elapsed_time = (float)(usecstop - usecstart) / 1000;
	float through_put = (float)numThread * BUFFER_SIZE * 5 / elapsed_time * 1000 / 1e6;

	printf("Elapsed time %.3f ms, %.3f MB/sec\n", elapsed_time, through_put);
	
	return 0;
}