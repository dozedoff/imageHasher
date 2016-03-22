/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include <Avahi.hpp>
#include "catch.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <memory>

using namespace imageHasher::discovery;

class AvahiTestFixture {
protected:
	std::unique_ptr<Avahi> cut;

	AvahiTestFixture() :cut(new Avahi()){
	}

	~AvahiTestFixture() {
	}
};

TEST_CASE_METHOD(AvahiTestFixture, "Avahi construction", "[AvahiTest]") {
	REQUIRE(cut != nullptr);
}

TEST_CASE_METHOD(AvahiTestFixture, "Avahi running", "[AvahiTest]") {
	REQUIRE(cut->is_running());
}
