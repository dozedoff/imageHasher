/*
 * ImageHasher - C++ version of the Hashing module from SimilarImage
 * Copyright (C) 2014  Nicholas Wright
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
	enum Status {OK, INVALID, FILTER, SHA, UNKNOWN};

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
	bool entryExists(fs::path);
	bool hasSHA(fs::path);
	std::string getSHA(fs::path filepath);
	std::list<fs::path> getFilesWithPath(fs::path);
	void prunePath(std::list<fs::path>);
	int getUserSchemaVersion();
	void setUserSchemaVersion(int version);
	static int getCurrentSchemaVersion();
	void exec(const char*);

private:
	sqlite3 *db;
	char* errMsg;
	std::list<db_data> dataA, dataB;
	std::list<db_data>* currentList;
	boost::mutex flipMutex, dbMutex;
	sqlite3_stmt *addOkStmt, *addInvalidStmt, *addFilterStmt, *startTrStmt, *checkExistsStmt, *checkSHAStmt, *commitTrStmt, *pruneQueryStmt, *pruneDeleteImageStmt, *pruneDeleteBadFileStmt, *updateShaStmt, *getSHAqueryStmt;
	log4cplus::Logger logger;
	unsigned int recordsWritten;
	bool running;
	boost::thread *workerThread;

	void init();
	void setupDatabase();
	void updateSchema();
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
