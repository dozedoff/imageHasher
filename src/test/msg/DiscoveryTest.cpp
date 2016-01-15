/*
 * DirectoryTest.cpp
 *
 *  Created on: 30 Dec 2015
 *      Author: nicholas
 */

#include <msg/Discovery.hpp>
#include "catch.hpp"
#include "zmq.hpp"
#include <memory>
#include <boost/asio.hpp>

using imageHasher::msg::Discovery;
using namespace boost::log::trivial;
using namespace boost::asio;

class DiscoveryTest {
protected:
	std::unique_ptr<imageHasher::msg::Discovery> cut;
	std::unique_ptr<zmq::context_t> context;
	std::unique_ptr<zmq::socket_t> request, publisher;

	io_service service;

	ip::udp::socket socket;

	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

	DiscoveryTest()
	:
		context(new zmq::context_t()),
		request(new zmq::socket_t(*context,ZMQ_REP)),
		publisher(new zmq::socket_t(*context,ZMQ_PUB)),
		cut(new imageHasher::msg::Discovery("127.0.0.1", 5555)),
		socket(service, ip::udp::endpoint(ip::udp::v4(), 9000))
	{
		request->bind("tcp://*:5555");
		publisher->bind("tcp://*:4444");
		cut->connect("127.0.0.1", 5555);
	}

	~DiscoveryTest() {
		publisher.get()->close();
		request.get()->close();
		context.get()->close();
		socket.close();
	}
};

TEST_CASE_METHOD(DiscoveryTest, "Construct Directory", "[DirectoryTest]") {
	REQUIRE(cut.get() != NULL);
}

TEST_CASE_METHOD(DiscoveryTest, "Register Service", "[DirectoryTest]") {
	Discovery::service_t foo = Discovery::service_t::compute;
	cut->register_node(foo);
	zmq::message_t msg;

	CHECK(request->recv(&msg));
	CHECK(msg.more());

	Discovery::request_t req_msg;
	memcpy(&req_msg, msg.data(), sizeof(Discovery::request_t));
	CHECK(req_msg == Discovery::request_t::hello);

	zmq::message_t msg2;
	Discovery::service_t service_msg;
	request->recv(&msg2);
	memcpy(&service_msg, msg2.data(), sizeof(Discovery::service_t));

	CHECK(service_msg == Discovery::service_t::compute);
}


