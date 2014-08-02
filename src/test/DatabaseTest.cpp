#include <gtest/gtest.h>
#include "../include/Database.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

class DatabaseTest : public :: testing::Test {
protected:

	Database* db;
	boost::filesystem::path* dbPath;

	boost::filesystem::path tempfile() {
		return boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path();
	}

	DatabaseTest() {
		dbPath = new boost::filesystem::path(tempfile());
		db = new Database(dbPath->c_str());
	}

	~DatabaseTest() {
		db->shutdown();
		delete(db);
	}
};

TEST_F(DatabaseTest, dbCreationCustom) {
	ASSERT_TRUE(boost::filesystem::exists(*dbPath));
}

TEST_F(DatabaseTest, writeRecords) {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar", "bar",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	ASSERT_EQ(2, db->getRecordsWritten());
}

TEST_F(DatabaseTest, writeRecordsWithInvalid) {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar", "bar",0);
	Database::db_data dbd3("notValid", "",0);

	dbd3.status = Database::INVALID;

	db->add(dbd1);
	db->add(dbd2);
	db->add(dbd3);

	db->shutdown();

	ASSERT_EQ(2, db->getRecordsWritten());
}

TEST_F(DatabaseTest, writeDuplicateRecords) {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("foo","foo",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	ASSERT_EQ(1, db->getRecordsWritten());
}

TEST_F(DatabaseTest, writeDuplicateRecordsDifferentPath) {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar","foo",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	ASSERT_EQ(2, db->getRecordsWritten());
}

TEST_F(DatabaseTest, entryExistsNonExistant) {
	Database::db_data dbd1("foo");

	ASSERT_FALSE(db->entryExists(dbd1));
}

TEST_F(DatabaseTest, entryExists) {
	Database::db_data dbd1("foo","foo",0);
	db->add(dbd1);
	db->flush();
	ASSERT_TRUE(db->entryExists(dbd1));
	db->shutdown();
}

TEST_F(DatabaseTest, getSHAvalid) {
	Database::db_data data("foobar", "ABCD", 1);

	db->add(data);
	db->flush();

	std::string shaHash = db->getSHA(fs::path("foobar"));

	ASSERT_EQ("ABCD", shaHash);
	db->shutdown();
}

TEST_F(DatabaseTest, getSHANotExisting) {
	std::string shaHash = db->getSHA(fs::path("unknown"));

	ASSERT_TRUE(shaHash.empty());
	db->shutdown();
}

TEST_F(DatabaseTest, add_path_placeholder) {
	int id = db->add_path_placeholder("placeholder");
	db->shutdown();

	ASSERT_EQ(1, id);
}
