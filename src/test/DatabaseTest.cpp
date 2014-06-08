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

	ASSERT_EQ(3, db->getRecordsWritten());
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

TEST_F(DatabaseTest, DISABLED_updateSHA) {
	Database::db_data data("foobar", "bar", 1);

	db->add(data);
	db->flush();

	std::string shaHash = db->getSHA(fs::path("foobar"));

	ASSERT_EQ("bar", shaHash);

	db->updateSHA256("foobar", "foo");

	shaHash = db->getSHA(fs::path("foobar"));

	ASSERT_EQ("foo", shaHash);
}

TEST_F(DatabaseTest, getDefaultUserSchemaVersion) {
	int version = db->getUserSchemaVersion();

	ASSERT_EQ(Database::getCurrentSchemaVersion(),version);
}

TEST_F(DatabaseTest, setUserSchemaVersion) {
	int version = db->getUserSchemaVersion();

	// guard
	ASSERT_EQ(Database::getCurrentSchemaVersion(), version);

	db->setUserSchemaVersion(42);
	version = db->getUserSchemaVersion();

	ASSERT_EQ(42, version);
}

TEST_F(DatabaseTest, dbNewerThanCurrent) {
	db->exec("PRAGMA locking_mode = NORMAL;");

	db->setUserSchemaVersion(42);
	int version = db->getUserSchemaVersion();

	ASSERT_EQ(42, version);

	db->shutdown();
	ASSERT_THROW(Database db2(dbPath->c_str()), std::runtime_error);
}

TEST_F(DatabaseTest, getSHAidNoEntries) {
	int shaId = db->getSHAid("foo");

	ASSERT_EQ(-1, shaId);
}

TEST_F(DatabaseTest, getSHAidNoMatchingEntries) {
	Database::db_data data("foobar", "bar", 42);
	db->add(data);
	db->flush();

	int shaId = db->getSHAid("foo");

	ASSERT_EQ(-1, shaId);
}

TEST_F(DatabaseTest, getSHAidMatchingEntry) {
	Database::db_data data("foobar", "bar", 42);
	Database::db_data data2("foo", "foo", 42);

	db->add(data);
	db->add(data2);
	db->flush();

	int shaId = db->getSHAid("foo");

	ASSERT_EQ(2, shaId);
}


