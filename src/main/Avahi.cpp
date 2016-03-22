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

namespace imageHasher {
namespace discovery {

Avahi::Avahi() : threaded_poll(nullptr), client(nullptr), group(nullptr){
	int error;
	this->threaded_poll = avahi_threaded_poll_new();
	// TODO add callback function instead of nullptr
	this->client = avahi_client_new(avahi_threaded_poll_get(this->threaded_poll),AVAHI_CLIENT_NO_FAIL,nullptr, nullptr, &error);
	avahi_threaded_poll_start(this->threaded_poll);
}

Avahi::~Avahi() {
	if (this->group) {
		avahi_entry_group_free(this->group);
	}

	if (this->client) {
		avahi_client_free(this->client);
	}

	if (this->threaded_poll) {
		avahi_threaded_poll_stop(this->threaded_poll);
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

} /* namespace discovery */
} /* namespace imageHasher */
