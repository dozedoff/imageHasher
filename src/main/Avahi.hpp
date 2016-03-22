/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file avahi.hpp
 *
 * @date 22 Mar 2016
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#ifndef SRC_MAIN_AVAHI_HPP_
#define SRC_MAIN_AVAHI_HPP_

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>
#include <avahi-common/thread-watch.h>

namespace imageHasher {
namespace discovery {

class Avahi {
public:
	Avahi();
	virtual ~Avahi();
	bool is_running();

private:
	AvahiEntryGroup *group;
	AvahiClient *client;
	AvahiThreadedPoll *threaded_poll;
};

} /* namespace discovery */
} /* namespace imageHasher */

#endif /* SRC_MAIN_AVAHI_HPP_ */
