/*
 * Database.hpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <sqlite3.h>

class Database {
public:
	Database();

	bool init();
private:
	bool initOk;
	sqlite3 *db;
	char* errMsg;

	void db_exec(char*);
	static int callback(void*, int, char**, char**);

};

#endif /* DATABASE_HPP_ */
