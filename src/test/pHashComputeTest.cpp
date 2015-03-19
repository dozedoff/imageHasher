/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <gtest/gtest.h>
#include "../main/pHashCompute.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include "zmq.hpp"

class pHashComputeTest : public :: testing::Test {
protected:
	imageHasher::pHashCompute *phc;

	pHashComputeTest() {
		phc = new imageHasher::pHashCompute(4,5555,"tcp://127.0.0.1:4444");
	}

	~pHashComputeTest() {
		delete(phc);
	}
};

TEST_F(pHashComputeTest, shutdown) {
	ASSERT_TRUE(phc != NULL);
}
