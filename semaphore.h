#include <pthread.h>

class Semaphore {
public:
	Semaphore(int n);
	void P(); // decrement count, make thread wait if below 0
	void V(); // increase count, wake up waiting threads
	
private:
	int count;
	pthread_mutex_t count_mutex;
	pthread_cond_t  cond_count_inc;
};
