#include <gtest/gtest.h>
#include "../include/Database.hpp"
#include <boost/filesystem.hpp>

TEST(DatabaseTest, dbCreation) {
	Database db;
	boost::filesystem::path dbPath("imageHasher.db");

	ASSERT_TRUE(boost::filesystem::exists(dbPath));
}
