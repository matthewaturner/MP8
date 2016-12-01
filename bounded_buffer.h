#include <list>
#include "semaphore.h"

/*---------------------------------------------------------------------------*/
/* Definition                                                                */
/*---------------------------------------------------------------------------*/

template <class T> class BoundedBuffer {
public:
	BoundedBuffer(int n)
		: full(0), empty(n), lock(1), elems() {}
		
	void push(T elem);
	T pop();
	int size() { return elems.size(); }
private:
	Semaphore full, empty, lock;
	std::list<T> elems;
};

/*---------------------------------------------------------------------------*/
/* Functions                                                                 */ 
/*---------------------------------------------------------------------------*/

template <class T> void BoundedBuffer<T>::push(T elem)
{
	empty.P(); // reduce number of empty spots
	lock.P();  // mutually exclusive access to elems
	elems.push_back(elem);
	lock.V();
	full.V();  // increase number of elems
}

template <class T> T BoundedBuffer<T>::pop()
{
	full.P();  // reduce number of elems
	lock.P();  // mutually exclusive access to elems
	T temp = elems.front();
	elems.pop_front();
	lock.V();
	empty.V(); // increase number of empty spots

	return temp;
}
