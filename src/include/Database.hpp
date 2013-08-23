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

class Database {
public:
	Database();
	~Database();
	enum Status {OK = 1, INVALID = 2};

	struct db_data {
		db_data() : filePath(""), pHash(0), status(INVALID) {};
		db_data(boost::filesystem::path path) : filePath(path), pHash(0), status(INVALID) {};
		boost::filesystem::path filePath;
		uint64_t pHash;
		Status status;
	};

	void add(db_data);
	unsigned int getRecordsWritten();
	void shutdown();
private:
	sqlite3 *db;
	char* errMsg;
	std::list<db_data> dataA, dataB;
	std::list<db_data>* currentList;
	boost::mutex flipMutex;
	sqlite3_stmt *addOkStmt, *addInvalidStmt, *startTrStmt, *checkExistsStmt, *commitTrStmt;
	log4cplus::Logger logger;
	unsigned int recordsWritten;
	bool running;
	boost::thread *workerThread;

	void init();
	void exec(const char*);
	void drain();
	void flipLists();
	void doWork();
	void prepareStatements();
	void addToBatch(db_data);
	int executeBatch();

	static int callback(void*, int, char**, char**);
};

#endif /* DATABASE_HPP_ */
