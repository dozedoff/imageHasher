/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <gtest/gtest.h>
#include "pHashCompute.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <cstring>
#include "zmq.hpp"
#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/magick/image.h>

class pHashComputeTest : public :: testing::Test {
protected:
	std::unique_ptr<imageHasher::pHashCompute> phc;

	zmq::context_t *context;
	zmq::socket_t *pull_socket, *push_socket;

	pHashComputeTest() {
		Magick::InitializeMagick(NULL);
		context = new zmq::context_t(1);

		pull_socket = new zmq::socket_t(*context, ZMQ_PULL);
		pull_socket->bind("tcp://*:5555");

		push_socket = new zmq::socket_t(*context, ZMQ_PUSH);
		push_socket->bind("tcp://*:4444");

		phc.reset(new imageHasher::pHashCompute("127.0.0.1",4444,5555,4));
	}

	~pHashComputeTest() {
		push_socket->close();
		pull_socket->close();
		context->close();

		phc.reset();
		delete(push_socket);
		delete(pull_socket);
		delete(context);
	}

	void send_message(unsigned int job_id, const void* data, int data_length);
};

void pHashComputeTest::send_message(unsigned int job_id, const void* data, int data_length) {
	zmq::message_t id(sizeof(unsigned int));
	zmq::message_t request(data_length);

	memcpy(id.data(), &job_id, sizeof(unsigned int));
	memcpy(request.data(),data, data_length);

	push_socket->send(id, ZMQ_SNDMORE);
	push_socket->send(request, 0);
}

TEST_F(pHashComputeTest, construction) {
	ASSERT_TRUE(phc.get() != NULL);
}

TEST_F(pHashComputeTest, push_socket_connected) {
	ASSERT_TRUE(push_socket->connected());
}

TEST_F(pHashComputeTest, pull_socket_connected) {
	ASSERT_TRUE(pull_socket->connected());
}

TEST_F(pHashComputeTest, hashImage_response_length) {
	Magick::Image img ("commoncpp/src/test/hash/testImage.jpg");
	Magick::Blob blob;
	img.write(&blob);

	this->send_message(0, blob.data(),blob.length());

	zmq::message_t id;
	zmq::message_t response;

	pull_socket->recv(&id);
	pull_socket->recv(&response);

	EXPECT_EQ(sizeof(unsigned int), id.size());
	ASSERT_EQ(sizeof(int64_t), response.size());
}

TEST_F(pHashComputeTest, hashImage_response_content) {
	Magick::Image img ("commoncpp/src/test/hash/testImage.jpg");
	Magick::Blob blob;
	img.write(&blob);

	this->send_message(42, blob.data(),blob.length());

	zmq::message_t id;
	zmq::message_t response;

	pull_socket->recv(&id);
	pull_socket->recv(&response);

	long pHash = 0;
	unsigned int job_id = 0;

	memcpy(&job_id, id.data(), sizeof(unsigned int));
	memcpy(&pHash, response.data(), sizeof(long));

	EXPECT_EQ(42U, job_id);
	ASSERT_EQ(4092185452341198848, pHash);
}

TEST_F(pHashComputeTest, hashImage_multiple_messages) {
	Magick::Image img ("commoncpp/src/test/hash/testImage.jpg");
	Magick::Blob blob;
	img.write(&blob);

	int msg_count = 10 * 2; // * 2 because of id + payload, two separate messages
	int send_counter = 0;
	int rcv_counter = 0;

	for(send_counter = 0; send_counter < msg_count; send_counter++) {
		this->send_message(send_counter, blob.data(), blob.length());
	}

	zmq::message_t response;
	while(rcv_counter < msg_count) {
		if(pull_socket->recv(&response, 1)) {
			response.rebuild();
			rcv_counter++;
		}
	}

	ASSERT_EQ(msg_count, rcv_counter);
}

TEST_F(pHashComputeTest, negative_thread_parameter) {
	EXPECT_THROW({phc.reset(new imageHasher::pHashCompute("127.0.0.1",4444,5555,-1));}, std::runtime_error);
}

TEST_F(pHashComputeTest, zero_thread_parameter) {
	EXPECT_THROW({phc.reset(new imageHasher::pHashCompute("127.0.0.1",4444,5555,-1));}, std::runtime_error);
}


