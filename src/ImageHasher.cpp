/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/*
 * imageHasher.cpp
 *
 *  Created on: 13 Aug 2013
 *      Author: nicholas
 */

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <boost/filesystem.hpp>

#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/Magick++/Exception.h>

#include <string>
#include <csignal>
#include <cstdlib>

#include "ImageFinder.hpp"
#include "HashWorker.hpp"
#include "hash/ImagePHash.hpp"

using namespace boost::log::trivial;
using namespace boost::filesystem;

//TODO Convert to C++ class (see HashUtil.cpp)



// Prototypes
path getPath(char**);
void signalHandler(int);
HashWorker *hw;

int main(int argc, char* argv[]) {
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
	ImageFinder imgF;
	ImagePHash iph(32,9);
	long imageCount = 0;

	signal(SIGINT, signalHandler);

	path searchPath = getPath(argv);
	std::list<path> imagePaths = imgF.getImages(searchPath);

	imageCount = imagePaths.size();
	BOOST_LOG_SEV(logger,info) << "Found " << imageCount << " images in " << searchPath;

	hw = new HashWorker(&imagePaths,4);
	BOOST_LOG_SEV(logger,info) << "Starting hashing of images...";
	hw->start();
}

path getPath(char* argv[]) {
	path path;
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
	if (NULL == argv[1]) {
		path = boost::filesystem::current_path();
		BOOST_LOG_SEV(logger,info) << "No search path given, using current directory " << path;
	} else {
		path = boost::filesystem::path(argv[1]);
		BOOST_LOG_SEV(logger,info) << "Search path is " << path;
	}

	return path;
}

void signalHandler(int signal) {
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
	BOOST_LOG_SEV(logger,info) << "Program interrupted, Shutting down";
	hw->clear();
}
