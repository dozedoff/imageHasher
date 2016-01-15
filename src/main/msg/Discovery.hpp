/*
 * Directory.h
 *
 *  Created on: 29 Dec 2015
 *      Author: nicholas
 */

#ifndef SRC_MAIN_MSG_DISCOVERY_HPP_
#define SRC_MAIN_MSG_DISCOVERY_HPP_

#include "zmq.hpp"

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <cstdint>

namespace imageHasher {
namespace msg {

class Discovery {
public:
	enum class service_t : uint8_t {source = 0, db = 1, compute = 2 };
	enum class request_t : uint8_t {hello = 0, bye = 1, config = 2, heartbeat = 3};
	enum class directory_status_t : uint8_t {ok = 200};

	Discovery(std::string ip, int port);
	virtual ~Discovery();

	void connect(std::string ip, int port);
	void register_node(service_t &type);
	bool unregister();

	void query_service(service_t type);

	void send_heartbeat();

private:
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

	std::shared_ptr<zmq::context_t> context;

	std::unique_ptr<zmq::socket_t> broadcast_rcv;
	std::unique_ptr<zmq::socket_t> directory_request;

	std::string create_address(std::string prefix, std::string ip, int port);
	zmq::message_t create_service_msg(const service_t &service);
	zmq::message_t create_request_msg(const request_t &request);
};

} /* namespace msg */
} /* namespace imageHasher */

#endif /* SRC_MAIN_MSG_DISCOVERY_HPP_ */
