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

#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/Magick++/Exception.h>

#include <string>

#include "../include/ImageFinder.hpp"
#include "../include/HashWorker.hpp"
#include "../../commoncpp/src/include/hash/ImagePHash.hpp"

using namespace log4cplus;
using namespace boost::filesystem;

Logger logger;

// Prototypes
path getPath(char**);

int main(int argc, char* argv[]) {
	ImageFinder imgF;
	ImagePHash iph(32,9);
	long imageCount = 0;

	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("ImageHasher"));

	path searchPath = getPath(argv);
	std::list<path> imagePaths = imgF.getImages(searchPath);

	imageCount = imagePaths.size();
	LOG4CPLUS_INFO(logger, "Found " << imageCount << " images in " << searchPath);

	HashWorker hw(&imagePaths,4);
	LOG4CPLUS_INFO(logger, "Starting hashing of images...");
	hw.start();
	LOG4CPLUS_INFO(logger, "Processed " << imageCount << " images");
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
