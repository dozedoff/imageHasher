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

TEST_F(DatabaseTest, get_files_with_path) {
	Database::db_data data0("/foo/bar", "ABCD", 1);
	Database::db_data data1("/foo/bar/bar", "ABCD", 1);
	Database::db_data data2("/foo/baz", "ABCD", 1);
	Database::db_data data3("/bar/foo", "ABCD", 1);
	Database::db_data data4("/red/fox", "ABCD", 1);

	db->add(data0);
	db->add(data1);
	db->add(data2);
	db->add(data3);
	db->add(data4);

	db->flush();

	fs::path search_path("/foo/");

	std::list<fs::path> paths = db->getFilesWithPath(search_path);
	ASSERT_EQ(3,paths.size());

	paths.sort();

	ASSERT_STREQ("/foo/bar", paths.front().string().c_str());
	ASSERT_STREQ("/foo/baz", paths.back().string().c_str());
}

TEST_F(DatabaseTest, prunePath) {
	Database::db_data data0("/foo/bar", "ABCD", 1);
	Database::db_data data1("/foo/bar/bar", "ABCD", 1);
	Database::db_data data2("/foo/baz", "ABCD", 1);
	Database::db_data data3("/bar/foo", "ABCD", 1);
	Database::db_data data4("/red/fox", "ABCD", 1);

	db->add(data0);
	db->add(data1);
	db->add(data2);
	db->add(data3);
	db->add(data4);

	db->flush();

	std::list<fs::path> to_delete;

	to_delete.push_back(fs::path("/foo/bar"));
	to_delete.push_back(fs::path("/foo/baz"));

	// guard
	ASSERT_TRUE(db->entryExists(to_delete.front()));
	ASSERT_TRUE(db->entryExists(to_delete.back()));

	db->prunePath(to_delete);

	ASSERT_FALSE(db->entryExists(to_delete.front()));
	ASSERT_FALSE(db->entryExists(to_delete.back()));
}

TEST_F(DatabaseTest, getPhash) {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	int64_t pHash = db->getPhash("/foo/bar");

	ASSERT_EQ(42, pHash);
}

TEST_F(DatabaseTest, getPhash_invalid_entry) {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	int64_t pHash = db->getPhash("/foo/baz");

	ASSERT_EQ(-1, pHash);
}

TEST_F(DatabaseTest, get_hash_phash) {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	imageHasher::db::table::Hash hash = db->get_hash(42);

	ASSERT_STREQ("ABCD", hash.get_sha256().c_str());
}

TEST_F(DatabaseTest, get_hash_phash_not_valid) {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	imageHasher::db::table::Hash hash = db->get_hash(12);

	ASSERT_FALSE(hash.is_valid());
}
