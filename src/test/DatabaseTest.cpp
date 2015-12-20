/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

#include "catch.hpp"
#include "Database.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

class DatabaseTestFixture {
protected:

	Database* db;
	boost::filesystem::path* dbPath;

	boost::filesystem::path tempfile() {
		return boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path();
	}

	DatabaseTestFixture() {
		dbPath = new boost::filesystem::path(tempfile());
		db = new Database(dbPath->generic_string());
	}

	~DatabaseTestFixture() {
		db->shutdown();
		delete(db);
		delete(dbPath);
	}
};

TEST_CASE_METHOD(DatabaseTestFixture, "dbCreationCustom", "[DatabaseTest]") {
	REQUIRE(boost::filesystem::exists(*dbPath));
}

TEST_CASE_METHOD(DatabaseTestFixture, "writeRecords", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar", "bar",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	REQUIRE(db->getRecordsWritten() == 2);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "writeRecordsWithInvalid", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar", "bar",0);
	Database::db_data dbd3("notValid", "",0);

	dbd3.status = Database::INVALID;

	db->add(dbd1);
	db->add(dbd2);
	db->add(dbd3);

	db->shutdown();

	REQUIRE(db->getRecordsWritten() == 2);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getInvalidFiles","[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar", "bar",0);
	Database::db_data dbd3("notValid", "",0);

	dbd3.status = Database::INVALID;

	db->add(dbd1);
	db->add(dbd2);
	db->add(dbd3);

	db->shutdown();

	REQUIRE(db->get_invalid_files());
}

TEST_CASE_METHOD(DatabaseTestFixture,  "writeDuplicateRecords", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("foo","foo",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	REQUIRE(db->getRecordsWritten() == 1);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "writeDuplicateRecordsDifferentPath", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar","foo",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	REQUIRE(db->getRecordsWritten() == 2);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getShaFound", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	Database::db_data dbd2("bar","foo",0);

	db->add(dbd1);
	db->add(dbd2);

	db->shutdown();

	REQUIRE(db->get_sha_found()== 1);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "entryExistsNonExistant", "[DatabaseTest]") {
	Database::db_data dbd1("foo");

	REQUIRE_FALSE(db->entryExists(dbd1));
}

TEST_CASE_METHOD(DatabaseTestFixture,  "entryExists", "[DatabaseTest]") {
	Database::db_data dbd1("foo","foo",0);
	db->add(dbd1);
	db->flush();
	REQUIRE(db->entryExists(dbd1));
	db->shutdown();
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getSHAvalid", "[DatabaseTest]") {
	Database::db_data data("foobar", "ABCD", 1);

	db->add(data);
	db->flush();

	std::string shaHash = db->getSHA(fs::path("foobar"));

	REQUIRE(shaHash == "ABCD");
	db->shutdown();
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getSHANotExisting", "[DatabaseTest]") {
	std::string shaHash = db->getSHA(fs::path("unknown"));

	REQUIRE(shaHash.empty());
	db->shutdown();
}

TEST_CASE_METHOD(DatabaseTestFixture,  "add_path_placeholder", "[DatabaseTest]") {
	int id = db->add_path_placeholder("placeholder");
	db->shutdown();

	REQUIRE(id == 1);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "get_files_with_path", "[DatabaseTest]") {
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
	REQUIRE(paths.size() == 3);

	paths.sort();

	REQUIRE(paths.front().string() == "/foo/bar");
	REQUIRE(paths.back().string() == "/foo/baz");
}

TEST_CASE_METHOD(DatabaseTestFixture,  "prunePath", "[DatabaseTest]") {
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
	CHECK(db->entryExists(to_delete.front()));
	CHECK(db->entryExists(to_delete.back()));

	db->prunePath(to_delete);

	REQUIRE_FALSE(db->entryExists(to_delete.front()));
	REQUIRE_FALSE(db->entryExists(to_delete.back()));
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getPhash", "[DatabaseTest]") {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	int64_t pHash = db->getPhash("/foo/bar");

	REQUIRE(pHash == 42);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "getPhash_invalid_entry", "[DatabaseTest]") {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	int64_t pHash = db->getPhash("/foo/baz");

	REQUIRE(pHash == -1);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "get_hash_phash", "[DatabaseTest]") {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	imageHasher::db::table::Hash hash = db->get_hash(42);

	REQUIRE(hash.get_sha256() == "ABCD");
}

TEST_CASE_METHOD(DatabaseTestFixture,  "get_hash_phash_not_valid", "[DatabaseTest]") {
	Database::db_data data("/foo/bar", "ABCD", 42);
	db->add(data);
	db->flush();

	imageHasher::db::table::Hash hash = db->get_hash(12);

	REQUIRE_FALSE(hash.is_valid());
}

TEST_CASE_METHOD(DatabaseTestFixture,  "prune_hash_table_prune_count", "[DatabaseTest]") {
	db->add(Database::db_data ("/foo/bar", "1", 41));
	db->add(Database::db_data ("/foo/bar2", "2", 42));
	db->add(Database::db_data ("/foo/bar3", "3", 43));
	db->add(Database::db_data ("/baz/bar", "4", 44));
	db->add(Database::db_data ("/baz/foo", "5", 45));

	db->flush();

	std::list<boost::filesystem::path> paths = db->getFilesWithPath(boost::filesystem::path("/foo/"));
	db->prunePath(paths);

	int pruned = db->prune_hash_table();

	REQUIRE(pruned == 3);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "prune_hash_table_prune_count_zero", "[DatabaseTest]") {
	db->add(Database::db_data ("/foo/bar", "1", 41));
	db->add(Database::db_data ("/foo/bar2", "2", 42));
	db->add(Database::db_data ("/foo/bar3", "3", 43));
	db->add(Database::db_data ("/baz/bar", "4", 44));
	db->add(Database::db_data ("/baz/foo", "5", 45));

	db->flush();

	int pruned = db->prune_hash_table();

	REQUIRE(pruned == 0);
}

TEST_CASE_METHOD(DatabaseTestFixture,  "prune_hash_table_hash_exists", "[DatabaseTest]") {
	db->add(Database::db_data ("/foo/bar", "1", 41));
	db->add(Database::db_data ("/foo/bar2", "2", 42));
	db->add(Database::db_data ("/foo/bar3", "3", 43));
	db->add(Database::db_data ("/baz/bar", "4", 44));

	db->flush();

	REQUIRE(db->sha_exists("1"));

	std::list<boost::filesystem::path> paths = db->getFilesWithPath(boost::filesystem::path("/foo/"));
	db->prunePath(paths);

	db->prune_hash_table();

	REQUIRE_FALSE(db->sha_exists("1"));
}
