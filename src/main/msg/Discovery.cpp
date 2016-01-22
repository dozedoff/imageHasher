/*
 * Directory.cpp
 *
 *  Created on: 29 Dec 2015
 *      Author: nicholas
 */

#include <msg/Discovery.hpp>
#include <sstream>
#include <memory>
#include <boost/array.hpp>

using namespace boost::log::trivial;
using namespace boost::asio;
using namespace boost::asio::ip;

namespace imageHasher {
namespace msg {

Discovery::Discovery(std::string ip, int port)
:
context(new zmq::context_t(1)),
broadcast_rcv(new zmq::socket_t(*context, ZMQ_SUB)),
directory_request(new zmq::socket_t(*context, ZMQ_REQ))
{

}


void Discovery::connect(std::string ip, int port) {
	std::string request_address = create_address("tcp://", ip, port);
	BOOST_LOG_SEV(logger,info)<< "Connecting to Directory at " << request_address;

	this->directory_request->connect(request_address.c_str());

	if(!this->directory_request->connected()) {
		throw new std::runtime_error("Failed to connect to Directory");
	}

	BOOST_LOG_SEV(logger,info)<< "Connected to Directory";

	//	(*this->broadcast_rcv).connect(create_address("tcp://", ip, port).c_str());
}

std::string Discovery::create_address(std::string prefix, std::string ip, int port) {
	std::stringstream ss;
	ss << prefix << ip << ":" << port;
	return ss.str();
}

Discovery::~Discovery() {
	(*this->broadcast_rcv).close();
	(*this->directory_request).close();
	(*this->context).close();
}

void Discovery::register_node(service_t &type) {
	request_t req = request_t::hello;

	zmq::message_t msg = create_request_msg(req);
	zmq::message_t msg_type = create_service_msg(type);

	this->directory_request->send(msg, ZMQ_SNDMORE);
	this->directory_request->send(msg_type);
}

zmq::message_t Discovery::create_service_msg(
		 const service_t &service) {
	zmq::message_t msg(sizeof(service_t));
	memcpy(msg.data(), &service, sizeof(service_t));
	return msg;

}

zmq::message_t Discovery::create_request_msg(
		const request_t &request_t) {
	zmq::message_t msg(sizeof(request_t));
	memcpy(msg.data(), &request_t, sizeof(request_t));
	return msg;
}

void Discovery::broadcast(request_t request) {
	boost::system::error_code error_code;
	udp::socket socket(io_service);
	socket.open(udp::v4());

	socket.set_option(udp::socket::reuse_address(true));
	socket.set_option(socket_base::broadcast(true));

	udp::endpoint receiver_endpoint(ip::address_v4::broadcast(), 9000);


	boost::array<request_t, 1> send_buf = { {request_t::hello} };
	socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

	socket.close();
}

} /* namespace msg */
} /* namespace imageHasher */


