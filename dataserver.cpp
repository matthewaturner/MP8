/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.h"
#include "netreqchannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(RequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

void * handle_data_requests(void * args) {

  RequestChannel * data_channel =  (RequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(RequestChannel & _channel, const string & _request) {
  _channel.cwrite("hello to you too");
}

void process_data(RequestChannel & _channel, const string &  _request) {
  usleep(1000 + (rand() % 5000));
  //_channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));
  _channel.cwrite(int2string(rand() % 100));
}

void process_newthread(RequestChannel & _channel, const string & _request) {
  int error;
  nthreads ++;

  // -- Name new data channel

  string new_channel_name = "data" + int2string(nthreads) + "_";
  //  cout << "new channel name = " << new_channel_name << endl;

  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  RequestChannel * data_channel = new RequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  cout << "starting new thread " << nthreads << endl;
  if (error = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(RequestChannel & _channel, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(_channel, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(_channel, _request);
  }
  else if (_request.compare(0, 9, "newthread") == 0) {
    process_newthread(_channel, _request);
  }
  else {
    _channel.cwrite("unknown request");
  }

}

void handle_process_loop(RequestChannel & _channel) {

  for(;;) {

    //cout << "Reading next request from channel (" << _channel.name() << ") ..." << flush;
      cout << flush;
    string request = _channel.cread();
    //cout << " done (" << _channel.name() << ")." << endl;
    //cout << "New request is " << request << endl;

    if (request.compare("quit") == 0) {
      _channel.cwrite("bye");
      usleep(10000);          // give the other end a bit of time.
      break;                  // break out of the loop;
    }

    process_request(_channel, request);
  }
  
}

void* connection_handler(void *arg) {

	/* the file descriptor of the socket we are communicating over */
	int fd = *(int *)arg;
	/* variables for requests and responses */
	char buf[127];
	std::string request, response;

	while(true) {
		
		// read a request from the network connection
		read(fd, buf, 127);
		request = buf;
		
		// quit when we receive a quit message
		if(request.compare("quit") == 0)
			break;
		
		// the code that generates the responses (given)
		usleep(1000 + (rand() % 5000));
		response = int2string(rand() % 100);

		// write the response back through the connection
		write(fd, response.c_str(), response.length()+1);
	}

	/* if this isn't closed we have hanging threads because they will 
	   still have open sockets */
	close(fd);

	std::cout << "Connection closed.\n";
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

	// argument handling
	int p = 22565;
	int b = 20;

	int opt;
	while((opt = getopt(argc, argv, "p:b:")) != -1) {
		switch(opt) {
		case 'p':
			p = (int) strtol(optarg, NULL, 0);
			if(p < 1) {
				std::cout << "invalid option p" << std::endl;
				return -1;
			}
			break;

		case 'b':
			b = (int) strtol(optarg, NULL, 0);
			if(b < 0) {
				std::cout << "invalid option b" << std::endl;
			return -1;
			}
			break;
		}
	}

	std::cout << "Starting server on port " << p << ", with backlog " << b << std::endl;

	// start the server
	NetworkRequestChannel server(p, connection_handler, b);

	/* make sure the server closes properly
	   however! we never reach this point if we close the program with CTRL-Z and
	   must close it manually */
	server.~NetworkRequestChannel();
}

