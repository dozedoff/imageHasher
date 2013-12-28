#include <gtest/gtest.h>
#include "../include/Database.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

boost::filesystem::path tempfile() {
	return boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path();
}

TEST(DatabaseTest, dbCreationCustom) {
	boost::filesystem::path dbPath(tempfile());
	Database db(dbPath.c_str());

	ASSERT_TRUE(boost::filesystem::exists(dbPath));
}

TEST(DatabaseTest, writeRecords) {
	boost::filesystem::path dbPath(tempfile());

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
	boost::filesystem::path dbPath(tempfile());

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
	boost::filesystem::path dbPath(tempfile());

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
	boost::filesystem::path dbPath(tempfile());

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");

	ASSERT_FALSE(db.entryExists(dbd1));
}

TEST(DatabaseTest, entryExists) {
	boost::filesystem::path dbPath(tempfile());

	Database db(dbPath.c_str());

	Database::db_data dbd1("foo");
	dbd1.status = Database::OK;
	db.add(dbd1);
	db.flush();
	ASSERT_TRUE(db.entryExists(dbd1));
	db.shutdown();
}

TEST(DatabaseTest, getSHAvalid) {
	Database db(tempfile().c_str());

	Database::db_data data;

	data.filePath = fs::path("foobar");
	data.sha256 = "ABCD";
	data.pHash = 1;
	data.status = Database::OK;

	db.add(data);
	db.flush();

	std::string shaHash = db.getSHA(fs::path("foobar"));

	ASSERT_EQ("ABCD", shaHash);
	db.shutdown();
}

TEST(DatabaseTest, getSHANotExisting) {
	Database db(tempfile().c_str());

	std::string shaHash = db.getSHA(fs::path("unknown"));

	ASSERT_TRUE(shaHash.empty());
	db.shutdown();
}

TEST(DatabaseTest, updateSHA) {
	Database db(tempfile().c_str());

	Database::db_data data;

	data.filePath = fs::path("foobar");
	data.sha256 = "";
	data.pHash = 1;
	data.status = Database::OK;

	db.add(data);
	db.flush();

	std::string shaHash = db.getSHA(fs::path("foobar"));

	ASSERT_TRUE(shaHash.empty());

	db.updateSHA256("foobar", "foo");

	shaHash = db.getSHA(fs::path("foobar"));

	ASSERT_EQ("foo", shaHash);
}
