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

#include <string>

#include "../include/ImageFinder.hpp"
#include "../../commoncpp/src/include/hash/ImagePHash.hpp"

using namespace log4cplus;
using namespace boost::filesystem;

Logger logger;

// Prototypes
path getPath(char**);

int main(int argc, char* argv[]) {
	ImageFinder imgF;
	ImagePHash iph;
	long long pHash = 0;

	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("ImageHasher"));

	path searchPath = getPath(argv);

	std::list<path> imagePaths = imgF.getImages(searchPath);

	for (std::list<path>::iterator itr = imagePaths.begin(); itr != imagePaths.end(); ++itr) {
		std::string filepath = itr->string();
		pHash = iph.getLongHash(filepath);
		LOG4CPLUS_INFO(logger, pHash << " - " << *itr);
	}
}

path getPath(char* argv[]) {
	path path;

	if (NULL == argv[1]) {
		path = boost::filesystem::current_path();
		LOG4CPLUS_INFO(logger, "No search path given, using current directory " << path);
	} else {
		path = boost::filesystem::path(argv[1]);
		LOG4CPLUS_INFO(logger, "Search path is " << path);
	}

	return path;
}
