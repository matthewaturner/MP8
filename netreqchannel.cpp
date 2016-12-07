#include "netreqchannel.h"

NetworkRequestChannel::NetworkRequestChannel(const std::string _serv_host_name, 
					     const unsigned short _port_no) {

	std::cout << "Connecting to server...";

	// create client socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		std::cerr << "Error : could not create client socket\n";
	
	// create server address object
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(_port_no);

	// error checking
	if(inet_pton(AF_INET, _serv_host_name.c_str(), &serv_addr.sin_addr) <= 0)
		std::cerr << "inet_pton error occurred\n";

	// connect socket to server address
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
		std::cerr << "Error : connect failed\n";
	else
		std::cout << "connected\n";
}

NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, 
                                             void *(*connection_handler)(void *),
					     int backlog) {
	// create server address object
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(_port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	int serv_addr_size = sizeof(serv_addr); // needed for later accept call

	// create server socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
		std::cerr << "Error : could not create server socket\n";
	
	// bind server socket to port
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		std::cerr << "Error : failed to bind socket to port\n";

	// listen for connections
	if(listen(sockfd, backlog) < 0)
		std::cerr << "Error : failed to listen to connections\n";

	while(true) {
		int *thread_sockfd = new int;  // argument we need in the connection handler
		pthread_t thread_id;           // thread id created for this connection

		// accept the new connection
		*thread_sockfd = accept(sockfd, (struct sockaddr *)&serv_addr,
		                        (socklen_t *)&serv_addr_size);

		std::cout << "Connection accepted.\n";

		// check that connection returned a valid file descriptor
		if(*thread_sockfd < 0) {
			std::cerr << "Error : accept failed\n";
			delete thread_sockfd;
			
			// kill the whole dataserver because it has been too long since
			// we had a connection
			break;
		}

		// create the thread which will handle this connection
		pthread_create(&thread_id, 0, connection_handler, (void *)thread_sockfd);

		// let the system kill this thread when it completes (don't wait to join)
		pthread_detach(thread_id);
	}

	std::cout << "Server exiting..." << std::endl;
}

NetworkRequestChannel::~NetworkRequestChannel() {
	close(sockfd);
}

/* ------------------------------------------------------------------------- */
/* Network Request Channel Functions                                         */
/* ------------------------------------------------------------------------- */

std::string NetworkRequestChannel::cread() {
	char buf[127];

	if(recv(sockfd, buf, 127, 0) < 0)
		std::cerr << "Error reading from channel\n";
	
	return std::string(buf);
}

int NetworkRequestChannel::cwrite(std::string _msg) {
	if(_msg.length() >= 127) {
		std::cerr << "Message too long for crwite\n";
		return -1;
	}

	if(send(sockfd, _msg.c_str(), _msg.length()+1, 0) < 0) {
		std::cerr << "Error writing message\n";
		return -1;
	}
}

std::string NetworkRequestChannel::send_request(std::string _request) {
	cwrite(_request);
	return cread();
}
