/*
 * imageHasher.cpp
 *
 *  Created on: 13 Aug 2013
 *      Author: nicholas
 */

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include <boost/filesystem.hpp>

using namespace log4cplus;
using namespace boost::filesystem;

Logger logger;

// Prototypes
path getPath(char**);

int main(int argc, char* argv[]) {
	 BasicConfigurator config;
	 config.configure();
	 logger = Logger::getInstance(LOG4CPLUS_TEXT("ImageHasher"));

	 path path = getPath(argv);
}

path getPath(char* argv[]) {
	 path path;

	 if(NULL == argv[1]) {
		 path = boost::filesystem::current_path();
		 LOG4CPLUS_INFO(logger, "No search path given, using current directory " << path);
	 }else{
		path = boost::filesystem::path(argv[1]);
		LOG4CPLUS_INFO(logger, "Search path is " << path);
	 }

	return path;
}
