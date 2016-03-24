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

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace imageHasher {
namespace discovery {

class Avahi {
public:
	Avahi();
	virtual ~Avahi();
	bool is_running();

private:
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;

	AvahiEntryGroup *group;
	AvahiClient *client;
	AvahiThreadedPoll *threaded_poll;

	static void entry_group_callback(AvahiEntryGroup *group, AvahiEntryGroupState state, void *userdata);

	const char * get_error_msg();
	static const char * error_msg_lookup(AvahiClient *client);
};

} /* namespace discovery */
} /* namespace imageHasher */

#endif /* SRC_MAIN_AVAHI_HPP_ */
