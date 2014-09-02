/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <gtest/gtest.h>

using namespace log4cplus;
using namespace std;

int main(int argc, char* argv[]) {
	 BasicConfigurator config;
	 config.configure();
::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
