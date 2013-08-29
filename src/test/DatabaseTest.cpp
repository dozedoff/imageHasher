#include <gtest/gtest.h>
#include "../include/Database.hpp"
#include <boost/filesystem.hpp>

TEST(DatabaseTest, dbCreation) {
	boost::filesystem::path dbPath("imageHasher.db");
	boost::filesystem::remove(dbPath);

	Database db;

	ASSERT_TRUE(boost::filesystem::exists(dbPath));
}

TEST(DatabaseTest, dbCreationCustom) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	ASSERT_TRUE(boost::filesystem::exists(dbPath));
}

TEST(DatabaseTest, writeRecords) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");
	Database::db_data dbd2("bar");

	dbd1.status = Database::OK;
	dbd2.status = Database::OK;

	db.add(dbd1);
	db.add(dbd2);

	db.shutdown();

	ASSERT_EQ(2, db.getRecordsWritten());
}

TEST(DatabaseTest, writeRecordsWithInvalid) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");
	Database::db_data dbd2("bar");
	Database::db_data dbd3("notValid");

	dbd1.status = Database::OK;
	dbd2.status = Database::OK;
	dbd3.status = Database::INVALID;

	db.add(dbd1);
	db.add(dbd2);
	db.add(dbd3);

	db.shutdown();

	ASSERT_EQ(3, db.getRecordsWritten());
}

TEST(DatabaseTest, writeDuplicateRecords) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");
	Database::db_data dbd2("foo");

	dbd1.status = Database::OK;
	dbd2.status = Database::OK;

	db.add(dbd1);
	db.add(dbd2);

	db.shutdown();

	ASSERT_EQ(1, db.getRecordsWritten());
}

TEST(DatabaseTest, entryExistsNonExistant) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");

	ASSERT_FALSE(db.entryExists(dbd1));
}

TEST(DatabaseTest, entryExists) {
	boost::filesystem::path dbPath("/tmp/test.db");
	boost::filesystem::remove(dbPath);

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");
	db.add(dbd1);
	db.flush();
	ASSERT_TRUE(db.entryExists(dbd1));
	db.shutdown();
}
