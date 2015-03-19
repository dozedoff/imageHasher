/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file pHashCompute.cpp
 *
 * @date 18 Mar 2015
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#include "pHashCompute.hpp"

#include <iostream>

using namespace log4cplus;

namespace imageHasher {
const std::string pHashCompute::listen_socket_client = "tcp://*:";
const std::string pHashCompute::listen_socket_worker = "inproc://workers";

pHashCompute::pHashCompute(int workers, int listen_port, std::string remote_host) {
	logger = Logger::getInstance(LOG4CPLUS_TEXT("pHashCompute"));

	setup_sockets(listen_port);
	create_threads(workers);
}

//TODO do ZMQ routing

void pHashCompute::setup_sockets(int listen_port) {
	this->context = new zmq::context_t(1);

	this->workers = new zmq::socket_t(*(this->context), ZMQ_DEALER);
	this->workers->bind(this->listen_socket_worker.c_str());
	LOG4CPLUS_INFO(logger, "Listening for clients on port " << listen_port);

	std::stringstream ss;
	ss << this->listen_socket_client << listen_port;
	std::string client_socket = ss.str();

	this->client = new zmq::socket_t(*(this->context), ZMQ_ROUTER);
	this->client->bind(client_socket.c_str());
}

void pHashCompute::create_threads(int num_of_threads) {
	LOG4CPLUS_INFO(logger, "Starting " << num_of_threads << " worker thread(s)");

	for(int i = 0; i < num_of_threads; i++) {
		boost::thread *t = new boost::thread(&pHashCompute::process_requests, this, i);
		this->worker_group.add_thread(t);
		LOG4CPLUS_INFO(logger, "Worker thread " << i << " started");
	}
}

void pHashCompute::process_requests(int worker_no) {
	zmq::socket_t worker_socket(*(this->context), ZMQ_REP);
	worker_socket.connect(this->listen_socket_worker.c_str());

	try {

	while(!boost::this_thread::interruption_requested()) {
		zmq::message_t request;
		worker_socket.recv (&request);
		std::cout << "Received request: [" << (char*) request.data() << "]" << std::endl;

		// Send reply back to client
		zmq::message_t reply (6);
		memcpy ((void *) reply.data (), "World", 6);
		worker_socket.send (reply);
	}

	} catch (zmq::error_t const &e) {
		LOG4CPLUS_ERROR(logger, "Worker terminated with " << e.what());
	}
}

pHashCompute::~pHashCompute() {
	this->client->close();
	this->workers->close();

	this->context->close();

	delete (this->client);
	delete (this->workers);
	delete (context);
}

} /* namespace imageHasher */
