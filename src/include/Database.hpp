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
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "../main/table/Hash.hpp"
#include "../main/table/Hash-odb.hxx"
#include "../main/table/ImageRecord.hpp"
#include "../main/table/ImageRecord-odb.hxx"

#include <string>

namespace fs = boost::filesystem;

class Database {
public:
	Database();
	Database(const char*);
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

private:
	odb::database *orm_db;

	char* errMsg;
	std::list<db_data> dataA, dataB;
	std::list<db_data>* currentList;
	boost::mutex flipMutex, dbMutex;
	sqlite3_stmt *addOkStmt, *addInvalidStmt, *addFilterStmt, *startTrStmt, *checkExistsStmt, *checkSHAStmt, *commitTrStmt, *pruneQueryStmt, *pruneDeleteImageStmt, *pruneDeleteBadFileStmt, *updateShaStmt, *getSHAqueryStmt, *getSHAidQueryStmt, *insertShaRecordQueryStmt, *insertpHashRecordQueryStmt, *getPhashQueryStmt, *getpHashidQuery;
	log4cplus::Logger logger;
	unsigned int recordsWritten;
	bool running;
	boost::thread *workerThread;

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
	bool is_db_initialised();
	void initialise_db();
};

#endif /* DATABASE_HPP_ */
