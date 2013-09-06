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
#include <boost/program_options.hpp>

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
namespace po = boost::program_options;

Logger logger;
HashWorker *hw;

// Prototypes
path getPath(char**);
void signalHandler(int);
void processPath(string);


int main(int argc, char* argv[]) {
	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("ImageHasher"));

	signal(SIGINT, signalHandler);

	po::options_description hidden("Hidden options");
	po::positional_options_description pos;

	pos.add("path", -1);
	hidden.add_options()
		("path", po::value<vector<string> >()->multitoken(), "Paths to process")
	;

	po::variables_map vm;

	po::store(po::command_line_parser(argc,argv).options(hidden).positional(pos).run(), vm);

	vector<string> paths = vm["path"].as<vector<string> >();

	if (paths.size() == 0) {
		path path = boost::filesystem::current_path();
		LOG4CPLUS_INFO(logger, "No search path given, using current directory " << path);
		paths.push_back(path.string());
	}

	for(vector<string>::iterator ite = paths.begin(); ite != paths.end(); ++ite){
		processPath(*ite);
	}
}

void processPath(string searchPath) {
	ImageFinder imgF;
	ImagePHash iph(32, 9);

	list<path> imagePaths = imgF.getImages(searchPath);

	long imageCount = imagePaths.size();
	LOG4CPLUS_INFO(logger, "Found " << imageCount << " images in " << searchPath);

	hw = new HashWorker(&imagePaths, 4);
	LOG4CPLUS_INFO(logger, "Starting hashing of images...");
	hw->start();
	delete hw;
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
