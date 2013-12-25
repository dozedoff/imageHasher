/*
 * Database.hpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <sqlite3.h>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/chrono.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

namespace fs = boost::filesystem;

class Database {
public:
	Database();
	Database(const char*);
	~Database();
	enum Status {OK, INVALID, FILTER, UNKNOWN};

	struct db_data {
		db_data() : filePath(""), pHash(0), sha256(""), reason(""), status(UNKNOWN) {};
		db_data(fs::path path) : filePath(path), pHash(0), sha256(""), reason(""), status(UNKNOWN) {};

		boost::filesystem::path filePath;
		uint64_t pHash;
		std::string sha256;
		std::string reason;
		Status status;
	};

	void add(db_data);
	void updateSHA256(std::string path, std::string sha);
	unsigned int getRecordsWritten();
	void shutdown();
	int flush();
	bool entryExists(db_data);
	std::list<fs::path> getFilesWithPath(fs::path);
	void prunePath(std::list<fs::path>);
private:
	sqlite3 *db;
	char* errMsg;
	std::list<db_data> dataA, dataB;
	std::list<db_data>* currentList;
	boost::mutex flipMutex, dbMutex;
	sqlite3_stmt *addOkStmt, *addInvalidStmt, *addFilterStmt, *startTrStmt, *checkExistsStmt, *commitTrStmt, *pruneQueryStmt, *pruneDeleteImageStmt, *pruneDeleteBadFileStmt, *updateShaStmt;
	log4cplus::Logger logger;
	unsigned int recordsWritten;
	bool running;
	boost::thread *workerThread;

	void init();
	void setupDatabase();
	void exec(const char*);
	int drain();
	void flipLists();
	void doWork();
	void prepareStatements();
	void createPreparedStatement(const char*&, sqlite3_stmt*&);
	void addToBatch(db_data);
	int executeBatch();
	static int callback(void*, int, char**, char**);
	void startTransaction();
	void commitTransaction();
};

#endif /* DATABASE_HPP_ */
