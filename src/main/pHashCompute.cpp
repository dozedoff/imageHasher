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
#include "commoncpp.hpp"
#include <iostream>

using namespace boost::log::trivial;

namespace imageHasher {
const std::string pHashCompute::server_socket = "tcp://";
const std::string pHashCompute::worker_push_socket = "inproc://workertasks";
const std::string pHashCompute::worker_pull_socket = "inproc://workerresults";
const std::string pHashCompute::worker_ready_socket = "inproc://workerready";

pHashCompute::pHashCompute(std::string server_ip, int remote_push_port, int remote_pull_port, int workers)
:
  context(new zmq::context_t(1)),
  worker_push(new zmq::socket_t(*(this->context), ZMQ_PUSH)),
  worker_pull(new zmq::socket_t(*(this->context), ZMQ_PULL)),
  worker_ready(new zmq::socket_t(*(this->context), ZMQ_PULL)),
  client_pull(new zmq::socket_t(*(this->context), ZMQ_PULL))
  {
	if(workers < 1) {
                throw std::runtime_error("Number of threads must be 1 or greater");
        }

	setup_sockets (server_ip,remote_push_port, remote_pull_port);
	create_threads(workers);
}

void pHashCompute::setup_sockets(std::string ip, int remote_push_port, int remote_pull_port) {
	this->worker_push->bind(this->worker_push_socket.c_str());
	this->worker_pull->bind(this->worker_pull_socket.c_str());
	this->worker_ready->bind(this->worker_ready_socket.c_str());

	// setup remote server sockets
	this->pull_addr = create_address(ip, remote_push_port);
	this->push_addr = create_address(ip, remote_pull_port);

	this->client_pull->connect(pull_addr.c_str());

	if(!this->client_pull->connected()) {
		throw new std::runtime_error("Failed to connect to client pull port");
	}
	BOOST_LOG_SEV(logger,info)<<"Connected to remote server " << pull_addr << " for pulling tasks";

	this->client_push.reset(new zmq::socket_t(*(this->context), ZMQ_PUSH));
	this->client_push->connect(push_addr.c_str());

	if(!this->client_push->connected()) {
			throw new std::runtime_error("Failed to connect to client pull port");
	}
	BOOST_LOG_SEV(logger,info)<<"Connected to remote server " << push_addr << " for pushing results";
}

std::string pHashCompute::create_address(std::string ip, int port) {
	std::stringstream ss;
	ss << this->server_socket << ip << ":" << port;
	return ss.str();
}

void pHashCompute::thread_ready_wait(int num_of_threads) {
	// wait for worker ready responses
	for (int response = 0; response < num_of_threads; response++) {
		zmq::message_t ready_msg;
		this->worker_ready->recv(&ready_msg);
		std::string msg((const char*)ready_msg.data());
		BOOST_LOG_SEV(logger,debug)<<msg << " is ready";
	}
}

void pHashCompute::thread_send_ready(const std::string& who, zmq::socket_t& ready) {
	int who_length = who.length() + 1;
	zmq::message_t ready_msg(who_length);
	memcpy(ready_msg.data(), (void*) (who.c_str()), who_length);
	ready.send(ready_msg);
}

void pHashCompute::create_threads(int num_of_threads) {
	BOOST_LOG_SEV(logger,info)<<"Starting task proxy thread";
	boost::thread *tp = new boost::thread(&pHashCompute::route_tasks, this);
	this->worker_group.add_thread(tp);

	BOOST_LOG_SEV(logger,info)<<"Starting result proxy thread";
	boost::thread *rp = new boost::thread(&pHashCompute::route_results, this);
	this->worker_group.add_thread(rp);

	thread_ready_wait(2);

	BOOST_LOG_SEV(logger,info)<<"Starting " << num_of_threads << " worker thread(s)";

	for(int i = 0; i < num_of_threads; i++) {
		boost::thread *t = new boost::thread(&pHashCompute::process_requests, this, i);
		this->worker_group.add_thread(t);
		BOOST_LOG_SEV(logger,info)<<"Worker thread " << i << " started";
	}

	thread_ready_wait(num_of_threads);
}

void pHashCompute::process_requests(int worker_no) {
	zmq::socket_t tasks(*(this->context), ZMQ_PULL);
	zmq::socket_t results(*(this->context), ZMQ_PUSH);
	zmq::socket_t ready(*(this->context), ZMQ_PUSH);

	ready.connect(this->worker_ready_socket.c_str());
	tasks.connect(this->worker_push_socket.c_str());
	results.connect(this->worker_pull_socket.c_str());

	ImagePHash iph(32,9);

	while ((ready.connected() && tasks.connected() && results.connected()) != true) {

	}

	std::stringstream ss;
	ss << "Worker " << worker_no;
	thread_send_ready(ss.str(), ready);

	try {

	while(!boost::this_thread::interruption_requested()) {
		zmq::message_t id;
		zmq::message_t request;
		unsigned int job_id = 0;

		tasks.recv (&id);

		if(! id.more()) {
			BOOST_LOG_SEV(logger,debug)<<"Worker " << worker_no << " request was not a multi-part message ";
			continue;
		}

		memcpy(&job_id, id.data(), sizeof(unsigned int));
		tasks.recv (&request);

		BOOST_LOG_SEV(logger,debug)<<"Worker " << worker_no << " got a request with id " << job_id << " and size " << request.size();
		Magick::Blob blob(request.data(),request.size());
		long pHash = iph.getLongHash(blob);

		// Send reply back to client
		zmq::message_t reply (sizeof(long));
		memcpy ((void *) reply.data (), &pHash, sizeof(long));
		BOOST_LOG_SEV(logger,debug)<<"Worker " << worker_no << " sending response for job " << job_id << " with size "  << reply.size();

		results.send(id, ZMQ_SNDMORE);
		results.send (reply, 0);
	}

	} catch (zmq::error_t const &e) {
		BOOST_LOG_SEV(logger,error) << "Worker terminated with " << e.what();
	}
}

void pHashCompute::route_tasks() {
	try {
		zmq::socket_t ready(*(this->context), ZMQ_PUSH);
		ready.connect(this->worker_ready_socket.c_str());

		while(!ready.connected()) {
		}

		thread_send_ready("Task router", ready);

		zmq::proxy(*this->client_pull,*this->worker_push, NULL);
	} catch (zmq::error_t const &e) {
		BOOST_LOG_SEV(logger,error) << "Task proxy terminated with " << e.what();
	}
}

void pHashCompute::route_results() {
	try {
		zmq::socket_t ready(*(this->context), ZMQ_PUSH);
		ready.connect(this->worker_ready_socket.c_str());

		while(!ready.connected()) {
		}

		thread_send_ready("Result router", ready);

		zmq::proxy(*this->worker_pull,*this->client_push, NULL);
	} catch (zmq::error_t const &e) {
		BOOST_LOG_SEV(logger,error) << "Result proxy terminated with " << e.what();
	}
}

pHashCompute::~pHashCompute() {
	this->client_pull->close();
	this->client_push->close();

	this->worker_pull->close();
	this->worker_push->close();
	this->worker_ready->close();

	this->context->close();
}

} /* namespace imageHasher */
