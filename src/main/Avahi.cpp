/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file Avahi.cpp
 *
 * @date 22 Mar 2016
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#include <Avahi.hpp>

using namespace boost::log::trivial;

namespace imageHasher {
namespace discovery {


Avahi::Avahi() : threaded_poll(nullptr), client(nullptr), group(nullptr){
	int error;
	this->threaded_poll = avahi_threaded_poll_new();
	avahi_threaded_poll_start(this->threaded_poll);
	// TODO add callback function instead of nullptr

	lock_poll();
	this->client = avahi_client_new(avahi_threaded_poll_get(this->threaded_poll),AVAHI_CLIENT_NO_FAIL,nullptr, nullptr, &error);
	unlock_poll();

	lock_poll();
	this->group = avahi_entry_group_new(this->client,entry_group_callback, &(this->logger));
	unlock_poll();

	if (this->group == nullptr) {
		throw std::runtime_error("Group is null");
	}

	BOOST_LOG_SEV(this->logger,info)<<"Avahi client startup complete";
}

Avahi::~Avahi() {
	if (this->threaded_poll) {
			avahi_threaded_poll_stop(this->threaded_poll);
	}

	if (this->group) {
		avahi_entry_group_free(this->group);
	}

	if (this->client) {
		avahi_client_free(this->client);
	}

	if (this->threaded_poll) {
		avahi_threaded_poll_free(this->threaded_poll);
	}
}

bool Avahi::is_running() {
	if (!this->client) {
		return false;
	}

	if (avahi_client_get_state(this->client) == AVAHI_CLIENT_S_RUNNING) {
		return true;
	}

	return false;
}

void Avahi::entry_group_callback(AvahiEntryGroup* group, AvahiEntryGroupState state, void* userdata) {
	//boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger = *(boost::log::sources::severity_logger<boost::log::trivial::severity_level>*)userdata;

    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
//        	BOOST_LOG_SEV(logger,info)<<"Entry group established";

            break;

        case AVAHI_ENTRY_GROUP_COLLISION :
//            BOOST_LOG_SEV(logger,warning)<<"Entry group name collision";
            break;

        case AVAHI_ENTRY_GROUP_FAILURE :
//        	BOOST_LOG_SEV(logger,error)<<"Entry group failure";
        	throw std::runtime_error(error_msg_lookup(avahi_entry_group_get_client(group)));
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}

const char* Avahi::get_error_msg() {
	return error_msg_lookup(this->client);
}

bool Avahi::add_service(std::string name, std::string type, uint16_t port) {
	lock_poll();
	avahi_entry_group_add_service(this->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,(AvahiPublishFlags)0, "foobar", "_ipp._tcp", "homenet", "localhost", port);
	unlock_poll();
	return true;
}

inline const char* Avahi::error_msg_lookup(AvahiClient* client) {
	return avahi_strerror(avahi_client_errno(client));
}

inline void Avahi::lock_poll() {
	avahi_threaded_poll_lock(this->threaded_poll);
}

inline void Avahi::unlock_poll() {
	avahi_threaded_poll_unlock(this->threaded_poll);
}

} /* namespace discovery */
} /* namespace imageHasher */
