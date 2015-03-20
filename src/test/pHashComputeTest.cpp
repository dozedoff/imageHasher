/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <gtest/gtest.h>
#include "../main/pHashCompute.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <cstring>
#include "zmq.hpp"
#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/magick/image.h>

class pHashComputeTest : public :: testing::Test {
protected:
	imageHasher::pHashCompute *phc;

	zmq::context_t *context;
	zmq::socket_t *pull_socket, *push_socket;

	pHashComputeTest() {
		Magick::InitializeMagick(NULL);
		context = new zmq::context_t(1);

		pull_socket = new zmq::socket_t(*context, ZMQ_PULL);
		pull_socket->bind("tcp://*:5555");

		push_socket = new zmq::socket_t(*context, ZMQ_PUSH);
		push_socket->bind("tcp://*:4444");

		phc = NULL;
		phc = new imageHasher::pHashCompute("127.0.0.1",4444,5555,4);
	}

	~pHashComputeTest() {
		push_socket->close();
		pull_socket->close();
		context->close();

		delete(phc);
		delete(push_socket);
		delete(pull_socket);
		delete(context);
	}
};

TEST_F(pHashComputeTest, construction) {
	ASSERT_TRUE(phc != NULL);
}

TEST_F(pHashComputeTest, push_socket_connected) {
	ASSERT_TRUE(push_socket->connected());
}

TEST_F(pHashComputeTest, pull_socket_connected) {
	ASSERT_TRUE(pull_socket->connected());
}

TEST_F(pHashComputeTest, DISABLED_hashImage_response_length) {
	Magick::Image img ("commoncpp/src/test/hash/testImage.jpg");
	Magick::Blob blob;
	img.write(&blob);

	zmq::message_t request(blob.length());

	memcpy(request.data(),blob.data(),blob.length());

	push_socket->send(request);

	zmq::message_t response;
	pull_socket->recv(&response);

	ASSERT_EQ(response.size(), 4);
}
