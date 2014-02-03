#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

//2GB
#define MEM 2000000000
//10MB
#define MEM_b 10000000

//sequential access the memory
void *sequential_access(void *block_size)
{
	int i;
	int size = (int)(long)block_size;
	char *a = (char*)malloc(sizeof(char) * MEM);
	int mem = size > 1 ? MEM : MEM_b;
	
	//copy memory block by block to new allocated memories
	for(i = 0; i < mem/size; i++)
	{
		char *b = (char*)malloc(sizeof(char) * size);
		memcpy(b, a + i * size, size);
	}
}

//random access the memory
void *random_access(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	char *a = (char*)malloc(sizeof(char) * MEM);
	int mem = size > 1 ? MEM : MEM_b;

	srand((unsigned)time(NULL));
	
	//copy memory randomly to new allocated memories
	for(i = 0; i < mem/size; i++)
	{
		ran = rand()%(MEM/size);
		char *b = (char*)malloc(sizeof(char) * size);
		memcpy(b, a + ran * size, size);
	}
}

int main(int argc, char const *argv[])
{
	//check the input parameters
	if(argc != 4)
	{
		printf("usage: %s <access type> <block size> <num of threads>\n", argv[0]);
		printf("access type:\nseq    sequential\nran    random\nblock size: 1b, 1kb or 1mb\n");
		exit(-1);
	}

	int i;
	//number of threads
	int numThread = atoi(argv[3]);
	pthread_t tid[numThread];
	
	//get block size
	int block_size;
	if(strcmp(argv[2], "1b") == 0)
	{
		block_size = 1;
	}
	else if(strcmp(argv[2], "1kb") == 0)
	{
		block_size = 1024;
	}
	else if(strcmp(argv[2], "1mb") == 0)
	{
		block_size = 1048576;
	}
	else
	{
		printf("Error block size.\n");
		exit(-1);
	}

	//variables to count the time
	struct timeval etstart, etstop;
	unsigned long long usecstart, usecstop;
	
	//start time
	gettimeofday(&etstart, NULL);

	//do memcpy for each thread
	for (i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "seq") == 0)
			pthread_create(&tid[i], NULL, sequential_access, (void*)(long)block_size);
		else if(strcmp(argv[1], "ran") == 0)
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
	
	//end time
	gettimeofday(&etstop, NULL);
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

	float elapsed_time = (float)(usecstop - usecstart) / 1000;
	//change memory size according to block size
	int mem = block_size > 1 ? MEM : MEM_b;
	float through_put = (float)numThread * mem / elapsed_time * 1000 / 1e6;

	printf("Elapsed time %.3f ms, %.3f MB/sec\n", elapsed_time, through_put);
	return 0;
}