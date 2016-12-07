/*
    File: reqchannel.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

*/

#ifndef _netreqchannel_H_                   // include file only once
#define _netreqchannel_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   R e q u e s t C h a n n e l */
/*--------------------------------------------------------------------------*/

/* Steps to create a network connection:
 1 - (Create) server socket
 2 - (Bind) the server socket to a specific port (on the server's machine)
 3 - (Listen) on the bound socket
 4 - (Create) the client side socket
 5 - (Connect) to the server socket
 6 - Server socket will (Accept) the client side connect call
*/

class NetworkRequestChannel {

public:

	typedef enum {SERVER_SIDE, CLIENT_SIDE} Side;
	typedef enum {READ_MODE, WRITE_MODE} Mode;

private:

	int sockfd;

public:

	NetworkRequestChannel(const std::string _serv_host_name, const unsigned short _port_no);
	/* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
	to the given port number at the given server host. THIS CONTRUCTOR IS CALLED
	BY THE CLIENT. */
	NetworkRequestChannel(const unsigned short _port_no, void *(*connection_handler)(void *),
	                      int backlog);
	/* Creates a SERVER-SIDE local copy of the channel that is accepting
	connections at the given port number. NOTE that multiple clients can be
	connected to the same server-side end of the request channel. Whenever a new
	connection comes in, it is accepted by the server, andt he given connection
	handler is invoked. The parameter to the connection handler is the file 
	descriptor of the slave socket returned by the accept call. NOTE that the
	connection handler does not want to deal with closing the socket. You will
	have to close the socket once the connection handler is done. */
	~NetworkRequestChannel();
	/* Destructor of the local copy of the channel. */
	std::string send_request(std::string _request);
	/* Send a string over the channel and wait for a reply. */
	std::string cread();
	/* Blocking read of data from the channel. Returns a string of characters 
	read from the channel. Returns NULL if read failed. */
	int cwrite(std::string msg);
	/* Write the data to the channel. The function returns the number of 
	characters written to the channel */
};

#endif
