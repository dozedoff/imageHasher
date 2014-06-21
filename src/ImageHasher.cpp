/*
 * ImageHasher - C++ version of the Hashing module from SimilarImage
 * Copyright (C) 2014  Nicholas Wright
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include <csignal>
#include <cstdlib>

#include "include/ImageFinder.hpp"
#include "include/HashWorker.hpp"
#include "../commoncpp/src/include/hash/ImagePHash.hpp"

using namespace log4cplus;
using namespace boost::filesystem;

Logger logger;

// Prototypes
path getPath(char**);
void signalHandler(int);
HashWorker *hw;

int main(int argc, char* argv[]) {
	ImageFinder imgF;
	ImagePHash iph(32,9);
	long imageCount = 0;

	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("ImageHasher"));

	signal(SIGINT, signalHandler);

	path searchPath = getPath(argv);
	std::list<path> imagePaths = imgF.getImages(searchPath);

	imageCount = imagePaths.size();
	LOG4CPLUS_INFO(logger, "Found " << imageCount << " images in " << searchPath);

	hw = new HashWorker(&imagePaths,4);
	LOG4CPLUS_INFO(logger, "Starting hashing of images...");
	hw->start();
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

void signalHandler(int signal) {
	LOG4CPLUS_INFO(logger, "Program interrupted, Shutting down");
	hw->clear();
}
