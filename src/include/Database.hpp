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

class Database {
public:
	Database();
	bool init();

	typedef struct db_dat {
		boost::filesystem::path filePath;
		uint64_t pHash;
	};
private:
	bool initOk;
	sqlite3 *db;
	char* errMsg;

	void db_exec(char*);
	bool db_add(db_dat);
	void db_drain(std::list<db_dat>&);
	void doWork();

	static int callback(void*, int, char**, char**);
};

#endif /* DATABASE_HPP_ */
