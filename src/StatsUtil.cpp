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

int main(int argc, char* argv[]) {
	StatsUtil* su = new StatsUtil();
	int exitValue = su->run(argc, argv);
	delete su;
	return exitValue;
}

int StatsUtil::run(int argc, char* argv[]) {
	return 0;
}

StatsUtil::~StatsUtil() {
	// TODO Auto-generated destructor stub
}
