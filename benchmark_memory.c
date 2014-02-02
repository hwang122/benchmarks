#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

//300MB
#define MEM 30000000

void *sequential_access(void *block_size)
{
	int i;
	int size = (int)(long)block_size;
	char *a = (char*)malloc(sizeof(char) * MEM);
	//char* b = (char*)malloc(sizeof(char) * size);

	for(i = 0; i < MEM/size; i++)
	{
		char *b = (char*)malloc(sizeof(char) * size);
		memcpy(b, a + i * size, size);
	}
}

void *random_access(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	char *a = (char*)malloc(sizeof(char) * MEM);
	//char* b = (char*)malloc(sizeof(char) * size);

	srand((unsigned)time(NULL));

	for(i = 0; i < MEM/size; i++)
	{
		ran = rand()%(MEM/size);
		char *b = (char*)malloc(sizeof(char) * size);
		memcpy(b, a + ran * size, size);
	}
}

int main(int argc, char const *argv[])
{
	if(argc != 4)
	{
		printf("usage: %s <access type> <block size> <num of threads>\n", argv[0]);
		printf("access type:\n-s    sequential\n-r    random\nblock size: 1b, 1kb or 1mb\n");
		exit(-1);
	}

	int i;
	int numThread = atoi(argv[3]);
	pthread_t tid[numThread];

	int block_size;
	if(strcmp(argv[2], "1b") == 0)
	{
		block_size = 1;
	}
	else if(strcmp(argv[2], "1kb") == 0)
	{
		block_size = 1000;
	}
	else if(strcmp(argv[2], "1mb") == 0)
	{
		block_size = 1000000;
	}
	else
	{
		printf("Error block size.\n");
		exit(-1);
	}

	//variables to count the time
	struct timeval etstart, etstop;
	unsigned long long usecstart, usecstop;

	gettimeofday(&etstart, NULL);

	//do memcpy for each thread
	for (i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "-s") == 0)
			pthread_create(&tid[i], NULL, sequential_access, (void*)(long)block_size);
		else if(strcmp(argv[1], "-r") == 0)
			pthread_create(&tid[i], NULL, random_access, (void*)(long)block_size);
		else
		{
			printf("Error memory access type.\n");
			exit(-1);
		}
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
	float through_put = (float)numThread * MEM / elapsed_time * 1000 / 1e6;

	printf("Elapsed time %.3f ms, %.3f MB/sec\n", elapsed_time, through_put);
	return 0;
}