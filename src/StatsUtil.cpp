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

using namespace log4cplus;

int main(int argc, char* argv[]) {
	StatsUtil* su = new StatsUtil();
	int exitValue = su->run(argc, argv);

	delete su;
	return exitValue;
}

StatsUtil::StatsUtil() {
	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashUtil"));
	db = new Database();
}

int StatsUtil::run(int argc, char* argv[]) {
	db->getAllPaths();
	return 0;
}

StatsUtil::~StatsUtil() {
	delete db;
}
