#include <stdio.h>
#include <pthread.h>

void * thread_summation(void * arg);

int sum = 0;

int main(int argc, char * argv[])
{
	pthread_t tid1, tid2;
	int range1[] = {1, 5};
	int range2[] = {6, 10};
	pthread_create(&tid1, NULL, thread_summation, (void *)range1);
	pthread_create(&tid2, NULL, thread_summation, (void *)range2);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("result = %d \n", sum);
	return 0;
}

void * thread_summation(void * arg)
{
	 int start = ((int*)arg)[0];
	 int  end = ((int*)arg)[1];

	 while (start <= end)
	 {
	 	sum += start;
		start++;
	 }

	 return NULL;
	
}
