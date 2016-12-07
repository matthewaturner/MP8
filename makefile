# makefile

all: dataserver client 
semaphore.o: semaphore.h semaphore.cpp
	g++ -std=c++11 -c -g semaphore.cpp

netreqchannel.o: netreqchannel.h netreqchannel.cpp
	g++ -std=c++11 -c -g netreqchannel.cpp

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -std=c++11 -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++ -std=c++11 -g -o dataserver dataserver.cpp netreqchannel.o reqchannel.o -lpthread

client: client_MP7.cpp reqchannel.o semaphore.o
	g++ -std=c++11 -g -o client client_MP7.cpp bounded_buffer.h netreqchannel.o reqchannel.o semaphore.o -lpthread

clean:
	rm *.o fifo* dataserver client output*
