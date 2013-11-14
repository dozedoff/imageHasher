/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * StatsUtil.hpp
 *
 *  Created on: 14 Nov 2013
 *      Author: Nicholas Wright
 */

#ifndef STATSUTIL_HPP_
#define STATSUTIL_HPP_

#include "../include/Database.hpp"

class StatsUtil {
public:
	StatsUtil();
	int run(int argc, char* argv[]);
	virtual ~StatsUtil();

private:
	Database* db;
};

#endif /* STATSUTIL_HPP_ */
