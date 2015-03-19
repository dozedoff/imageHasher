/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file pHashCompute.hpp
 *
 * @date 18 Mar 2015
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#ifndef SRC_MAIN_PHASHCOMPUTE_HPP_
#define SRC_MAIN_PHASHCOMPUTE_HPP_

#include "zmq.hpp"

#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

namespace imageHasher {

class pHashCompute {
public:
	pHashCompute(int workers, int listen_port, std::string remote_host);
	virtual ~pHashCompute();

private:
	static const std::string listen_socket_client;
	static const std::string listen_socket_worker;

	log4cplus::Logger logger;

	zmq::context_t *context;
	zmq::socket_t *workers;
	zmq::socket_t *client;

	boost::thread_group worker_group;

	void setup_sockets(int listen_port);
	void create_threads(int num_of_threads);
	void process_requests(int worker_no);
};

} /* namespace imageHasher */

#endif /* SRC_MAIN_PHASHCOMPUTE_HPP_ */
