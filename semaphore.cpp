#include "semaphore.h"

Semaphore::Semaphore(int n)
{
	count = n;
	count_mutex = PTHREAD_MUTEX_INITIALIZER;
	cond_count_inc = PTHREAD_COND_INITIALIZER;
}

void Semaphore::P()
{
	pthread_mutex_lock(&count_mutex);
	count--;
	if(count < 0)
		pthread_cond_wait(&cond_count_inc, &count_mutex);
	pthread_mutex_unlock(&count_mutex);
}

void Semaphore::V()
{
	pthread_mutex_lock(&count_mutex);
	count++;
	if(count <= 0) 
		pthread_cond_signal(&cond_count_inc);
	pthread_mutex_unlock(&count_mutex);
}
