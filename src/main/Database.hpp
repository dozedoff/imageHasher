/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/*
 * Database.hpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/chrono.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include "table/Hash.hpp"
#include "table/Hash-odb.hxx"
#include "table/ImageRecord.hpp"
#include "table/ImageRecord-odb.hxx"

#include "PreparedQuery.hpp"

#include <string>
#include <memory>

namespace fs = boost::filesystem;

class Database {
public:
	Database();
	Database(const char*)__attribute__((deprecated));
	Database(const std::string db_name);
	~Database();
	enum Status {OK, INVALID, FILTER, SHA, UNKNOWN};

	struct db_data {
		db_data() : filePath(""), pHash(0), sha256(""), reason(""), status(UNKNOWN) {};
		db_data(fs::path path) : filePath(path), pHash(0), sha256(""), reason(""), status(UNKNOWN) {};
		db_data(fs::path path, std::string sha, int64_t ph) : filePath(path), pHash(ph), sha256(sha), reason(""), status(OK) {};

		boost::filesystem::path filePath;
		uint64_t pHash;
		std::string sha256;
		std::string reason;
		Status status;
	};

	void add(db_data);
	unsigned int getRecordsWritten();
	unsigned int get_sha_found();
	unsigned int get_invalid_files();
	void shutdown();
	int flush();
	bool entryExists(db_data);
	bool entryExists(fs::path);
	std::string getSHA(fs::path filepath);
	int64_t getPhash(fs::path filepath);
	std::list<fs::path> getFilesWithPath(fs::path);
	void prunePath(std::list<fs::path>);
	void exec(const char*);
	bool sha_exists(std::string sha);
	int add_path_placeholder(std::string path);
	imageHasher::db::table::Hash get_hash(std::string sha);
	imageHasher::db::table::Hash get_hash(u_int64_t phash);
	imageHasher::db::table::ImageRecord get_imagerecord(fs::path filepath);
	int prune_hash_table();

private:
	std::unique_ptr<odb::database> orm_db;

	std::list<db_data> dataA, dataB;
	std::list<db_data> currentList;
	boost::mutex flipMutex, dbMutex;
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
	unsigned int recordsWritten;
	unsigned int sha_found;
	unsigned int invalid_files;
	bool running;
	boost::thread *workerThread;
	imageHasher::db::PreparedQuery *prep_query;

	std::string dbName = "imageHasher.db";
	const std::string prune_hash_table_query = "DELETE FROM hash WHERE hash_id IN (SELECT hash_id FROM (SELECT imagerecord.hash, hash.hash_id FROM hash LEFT OUTER JOIN imagerecord  ON hash = hash_id) WHERE hash IS null);";

	void init();
	void setupDatabase();
	int drain();
	void flipLists();
	void doWork();
	void prepareStatements();
	void createPreparedStatement(const char*&, sqlite3_stmt*&);
	void addToBatch(db_data);
	int executeBatch();
	static int callback(void*, int, char**, char**);
	imageHasher::db::table::Hash addHashEntry(std::string sha, u_int64_t pHash);
	void add_record(db_data data);
	void add_invalid(db_data data);
	void add_filter(db_data data);
	bool is_db_initialised();
	void initialise_db();
};

#endif /* DATABASE_HPP_ */
