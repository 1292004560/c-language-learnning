#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void * thread_increse(void * arg);
void * thread_decrease(void *arg);

pthread_mutex_t mutex;

long long number = 0;

int main(int argc, char * argv[])
{
	pthread_t thread_ids[NUM_THREAD];

	int i;
	pthread_mutex_init(&mutex, NULL);

	printf("sizeof long long : %d \n", sizeof(long long));
	for (i = 0; i < NUM_THREAD; i++)
	{
		if (i%2)
			pthread_create(&(thread_ids[i]), NULL, thread_increse, NULL);
		else
			pthread_create(&(thread_ids[i]), NULL, thread_decrease, NULL);
		

	}

	for (i = 0; i < NUM_THREAD; i++)
		pthread_join(thread_ids[i], NULL);
	
	printf("result : %lld \n", number);

	pthread_mutex_destroy(&mutex);
	return 0;
}

void * thread_increse(void * arg)
{
	int i;
	for (i = 0; i < 50000000; i++)
	{
		pthread_mutex_lock(&mutex);
		number += 1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
void * thread_decrease(void * arg)
{
	int i;
	for (i = 0; i < 50000000; i++)
	{
		pthread_mutex_lock(&mutex);
		number -= 1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
