/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <gtest/gtest.h>
#include "../main/pHashCompute.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include "zmq.hpp"

class pHashComputeTest : public :: testing::Test {
protected:
	imageHasher::pHashCompute *phc;

	zmq::context_t *context;
	zmq::socket_t *pull_socket;

	pHashComputeTest() {
		context = new zmq::context_t(1);
		pull_socket = new zmq::socket_t(*context, ZMQ_PULL);
		pull_socket->bind("tcp://*:4444");

		phc = NULL;
		phc = new imageHasher::pHashCompute(4,5555,"tcp://127.0.0.1:4444");
	}

	~pHashComputeTest() {
		delete(phc);

		pull_socket->close();
		context->close();

		delete(pull_socket);
		delete(context);
	}
};

TEST_F(pHashComputeTest, construction) {
	ASSERT_TRUE(phc != NULL);
}
