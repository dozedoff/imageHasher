/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * StatsUtil.cpp
 *
 *  Created on: 14 Nov 2013
 *      Author: Nicholas Wright
 */

#include "include/StatsUtil.hpp"

#include <log4cplus/configurator.h>

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>


using namespace log4cplus;
using namespace boost;

namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
	StatsUtil* su = new StatsUtil();
	int exitValue = su->run(argc, argv);

	delete su;
	return exitValue;
}

StatsUtil::StatsUtil () : db(0) {
	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashUtil"));
	db = new Database();
}

int StatsUtil::run(int argc, char* argv[]) {
	std::list<fs::path> paths = db->getAllPaths();

	return 0;
}

std::list<std::string> StatsUtil::tokenizeFilename(boost::filesystem::path filepath) {
	std::string filename = filepath.filename().string();
	std::list<std::string> tokens;

	//TODO clip file endings
	char_separator<char> separator("_ -");

	tokenizer<char_separator<char> > token(filename, separator);

	BOOST_FOREACH (const std::string& t, token) {
	        tokens.push_back(t);
	}

	return tokens;
}

StatsUtil::~StatsUtil() {
	delete db;
}


