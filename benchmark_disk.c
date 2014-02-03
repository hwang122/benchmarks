#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 1000000000

//random access read the disk
void *random_read(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	srand((unsigned)time(NULL));
	
	int fd;
	//open binary file
	fd = open("test.bin", O_RDONLY, 0666);
	if(fd < 0)
	{
		printf("Error: open\n");
		exit(-1);
	}

	int capacity = (size == 1) ? (CAPACITY / 50) :CAPACITY;
	//randomly read the disk
	for(i = 0; i < capacity / size; i++)
	{
		ran = rand()%(capacity / size);
		char *buffer = (char*)malloc(sizeof(char) * size);
		//randomly move the indicator of file stream
		lseek(fd, ran * size, SEEK_SET);
		//read block size of file from the disk
		read(fd, buffer, size);
		//fgets(buffer, size, pFile);
		//return the indicator to the start of file
		//lseek(fd, 0, SEEK_SET);
	}

	close(fd);
}

//random access write the disk
void *random_write(void *block_size)
{
	int i, ran;
	int size = (int)(long)block_size;
	srand((unsigned)time(NULL));
	
	//open binary file
	int fd;
	fd = open("test.bin", O_CREAT|O_TRUNC|O_WRONLY, 0666);
	if(fd < 0)
	{
		printf("Error: open\n");
		exit(-1);
	}
	
	int capacity = (size == 1) ? (CAPACITY / 50) :CAPACITY;
	//randomly write the disk
	for(i = 0; i < capacity / size; i++)
	{
		ran = rand()%(capacity / size);
		char *buffer = (char*)malloc(sizeof(char) * size);
		memset(buffer, 'a', size);
		//randomly move the indicator of file stream
		lseek(fd, ran * size, SEEK_SET);
		//write block size of file to the disk
		write(fd, buffer, size);
		//return the indicator to the start of file
		//lseek(fd, 0, SEEK_SET);
	}

	close(fd);
}

//sequential access read the disk
void *sequential_read(void *block_size)
{
	int i;
	int size = (int)(long)block_size;
	
	//open binary file
	int fd;
	fd = open("test.bin", O_RDONLY, 0666);
	if(fd < 0)
	{
		printf("Error: open\n");
		exit(-1);
	}
	
	int capacity = (size == 1) ? (CAPACITY / 50) :CAPACITY;
	//sequential read the disk
	for(i = 0; i < capacity / size; i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * size);
		//after fread, the indicator of stream would be set to the current location
		//there is no need to move the indicator like randomly access
		read(fd, buffer, size);
	}

	close(fd);
}

//sequential access write the disk
void *sequential_write(void *block_size)
{
	int i;
	int size = (int)(long)block_size;
	
	//open binary file
	int fd;
	fd = open("test.bin", O_CREAT|O_TRUNC|O_WRONLY, 0666);
	if(fd < 0)
	{
		printf("Error: open\n");
		exit(-1);
	}
	
	int capacity = (size == 1) ? (CAPACITY / 50) :CAPACITY;
	//sequential write the disk
	for(i = 0; i < capacity / size; i++)
	{
		char *buffer = (char*)malloc(sizeof(char) * size);
		memset(buffer, 'a', size);
		//after fwrite, the indicator of stream would be set to the current location
		//there is no need to move the indicator like randomly access
		write(fd, buffer, size);
	}

	close(fd);
}

int main(int argc, char const *argv[])
{
	//check the input parameters
	if(argc != 5)
	{
		printf("usage: %s <operation type> <access type> <block size> <num of threads>\n", argv[0]);
		printf("operation type: \nRead    Read disk\nWrite    Write disk   \
			\naccess type: \nseq    sequential\nran    random    \
			\nblock size: 1b, 1kb, 1mb, 1gb\n");
		exit(-1);
	}

	int i;
	//number of threads
	int numThread = atoi(argv[4]);
	pthread_t tid[numThread];
	
	//set the block size
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
	
	//start time
	gettimeofday(&etstart, NULL);
	
	//do multi read or write at the same time
	for(i = 0; i < numThread; i++)
	{
		//read
		if(strcmp(argv[1], "Read") == 0)
		{
			//sequential
			if(strcmp(argv[2], "seq") == 0)
			{
				pthread_create(&tid[i], NULL, sequential_read, (void*)(long)block_size);
			}
			//random
			else if(strcmp(argv[2], "ran") == 0)
			{
				pthread_create(&tid[i], NULL, random_read, (void*)(long)block_size);
			}
			else
			{
				printf("Error access type.\n");
				exit(-1);
			}
		}
		//write
		else if(strcmp(argv[1], "Write") == 0)
		{
			//sequential
			if(strcmp(argv[2], "seq") == 0)
			{
				pthread_create(&tid[i], NULL, sequential_write, (void*)(long)block_size);
			}
			//random
			else if(strcmp(argv[2], "ran") == 0)
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
	
	//end time
	gettimeofday(&etstop, NULL);
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

	int capacity = (block_size == 1) ? (CAPACITY / 50) :CAPACITY;
	float elapsed_time = (float)(usecstop - usecstart) / 1000;
	float through_put = (float)numThread * capacity / elapsed_time * 1000 / 1e6;

	printf("Elapsed time %.3f ms, %.3f MB/sec\n", elapsed_time, through_put);
	return 0;
}