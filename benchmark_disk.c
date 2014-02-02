#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define CAPACITY 1000000000

FILE *pFile;

void *random_read(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	srand((unsigned)time(NULL));

	pFile = fopen("test.bin", "rb");
	if(pFile == NULL)
	{
		printf("FILE error\n");
		exit(-1);
	}

	for(i = 0; i < CAPACITY / size; i++)
	{
		ran = rand()%(CAPACITY / size);
		char *buffer = (char*)malloc(sizeof(char) * size);
		fseek(pFile, ran * size, SEEK_SET);
		fread(buffer, 1, size, pFile);
		rewind(pFile);
	}
}

void *random_write(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	srand((unsigned)time(NULL));

	pFile = fopen("test.bin", "wb");
	if(pFile == NULL)
	{
		printf("FILE error\n");
		exit(-1);
	}

	for(i = 0; i < CAPACITY / size; i++)
	{
		ran = rand()%(CAPACITY / size);
		char *buffer = (char*)malloc(sizeof(char) * size);
		memset(buffer, 'a', size);
		fseek(pFile, ran * size, SEEK_SET);
		fwrite(buffer, 1, size, pFile);
		rewind(pFile);
	}
}

void *sequential_read(void *block_size)
{
	int i;
	int size = (int)(long)block_size;

	pFile = fopen("test.bin", "rb");
	if(pFile == NULL)
	{
		printf("FILE error\n");
		exit(-1);
	}

	for(i = 0; i < CAPACITY / size; i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * size);
		fread(buffer, 1, size, pFile);
	}
}

void *sequential_write(void *block_size)
{
	int i;
	int size = (int)(long)block_size;

	pFile = fopen("test.bin", "wb");
	if(pFile == NULL)
	{
		printf("FILE error\n");
		exit(-1);
	}

	for(i = 0; i < CAPACITY / size; i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * size);
		memset(buffer, 'a', size);
		fwrite(buffer, 1, size, pFile);
	}
}

int main(int argc, char const *argv[])
{
	if(argc != 5)
	{
		printf("usage: %s <operation type> <access type> <block size> <num of threads>\n", argv[0]);
		printf("operation type: \n-R    read\n-W    write    \
			\naccess type: \n-s    sequential\n-r    random    \
			\nblock size: 1b, 1kb, 1mb, 1gb\n");
		exit(-1);
	}

	int i;
	int numThread = atoi(argv[4]);
	pthread_t tid[numThread];

	int block_size;
	if(strcmp(argv[3], "1b") == 0)
	{
		block_size = 1;
	}
	else if(strcmp(argv[3], "1kb") == 0)
	{
		block_size = 1000;
	}
	else if(strcmp(argv[3], "1mb") == 0)
	{
		block_size = 1000000;
	}
	else if (strcmp(argv[3], "1gb") == 0)
	{
		block_size = 1000000000;
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

	for(i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "-R") == 0)
		{
			if(strcmp(argv[2], "-s") == 0)
			{
				pthread_create(&tid[i], NULL, sequential_read, (void*)(long)block_size);
			}
			else if(strcmp(argv[2], "-r") == 0)
			{
				pthread_create(&tid[i], NULL, random_read, (void*)(long)block_size);
			}
			else
			{
				printf("Error access type.\n");
				exit(-1);
			}
		}
		else if(strcmp(argv[1], "-W") == 0)
		{
			if(strcmp(argv[2], "-s") == 0)
			{
				pthread_create(&tid[i], NULL, sequential_write, (void*)(long)block_size);
			}
			else if(strcmp(argv[2], "-r") == 0)
			{
				pthread_create(&tid[i], NULL, random_write, (void*)(long)block_size);
			}
			else
			{
				printf("Error access type.\n");
				exit(-1);
			}
		}
		else
		{
			printf("Error operation type.\n");
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
	float through_put = (float)numThread * CAPACITY / elapsed_time * 1000 / 1e6;

	printf("Elapsed time %.3f ms, %.3f MB/sec\n", elapsed_time, through_put);
	return 0;
}