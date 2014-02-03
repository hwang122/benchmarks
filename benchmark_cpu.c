#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

//number of operations
#define OPT 5000000000
#define PI 3.14

//Do OPT number of float operations
void *FLOPS()
{
	double a = 0.1, b = 0.2, c = 0.3, d = 0.4, e = 0.5;
	int i;
	//each loop contains 5 float operations
	//run multiple instructions concurrently
	for(i = 0; i < OPT/5; i++)
	{
		a += PI; b += PI; c += PI;
		d *= PI; e *= PI;
	}
}

//Do OPT number of integer operations
void *IOPS()
{
	int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6;
	int i;
	//each loop contains 5 integer operations
	//run multiple instructions concurrently
	for(i = 0; i < OPT/5; i++)
	{
		a += f; b += f; c += f;
		d *= f; e *= f;
	}
}

int main(int argc, char const *argv[])
{
	//check the input parameters
	if(argc != 3)
	{
		printf("usage: %s <operation type> <num of threads>\n", argv[0]);
		printf("operation type: \niops    IOPS\nflops    FLOPS\n");
		exit(-1);
	}

	int i;
	//get number of threads
	int numThread = atoi(argv[2]);
	pthread_t tid[numThread];

	//variables to count the time
	struct timeval etstart, etstop;
	unsigned long long usecstart, usecstop;
	
	//get the start time
	gettimeofday(&etstart, NULL);

	//run several threads at the same time
	for(i = 0; i < numThread; i++)
	{
		if(strcmp(argv[1], "iops") == 0)
			pthread_create(&tid[i], NULL, IOPS, NULL);
		else if(strcmp(argv[1], "flops") == 0)
			pthread_create(&tid[i], NULL, FLOPS, NULL);
		else
		{
			printf("Error operation type\n");
			exit(-1);
		}
	}
	
	//after doing operations, wait until all the threads finish
	//synchronization
	for (i = 0; i < numThread; i++)
	{
		pthread_join(tid[i], NULL);
	}
	
	//get the end time
	gettimeofday(&etstop, NULL);
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

	//FLOPS or IOPS equals to total number of operations divided by using time
	float elapsed_time = (float)(usecstop - usecstart) / 1000;
	float speed = (float)numThread * OPT / elapsed_time * 1000 / 1e9;

	printf("Elapsed time %.3f ms, %.3f Gflops/Giops\n", elapsed_time, speed);
	return 0;
}