/*
 * Database.cpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#include "../include/Database.hpp"


Database::Database() {
	this->initOk = false;
	this->db = 0; //TODO is this correct / safe?
	this->errMsg = 0;
}

int Database::callback(void *NotUsed, int argc, char **argv, char **arg2){
   return 0;
}

void Database::db_exec(char* command) {
	sqlite3_exec(db, command, callback, 0, &errMsg);
}

bool Database::init() {
	int ret = sqlite3_open("imageHasher.db", &db);
                db_exec(const_cast<char *>("PRAGMA page_size = 4096;"));
                db_exec(const_cast<char *>("PRAGMA cache_size=10000;"));
                db_exec(const_cast<char *>("PRAGMA locking_mode=EXCLUSIVE;"));
                db_exec(const_cast<char *>("PRAGMA synchronous=NORMAL;"));
                db_exec(const_cast<char *>("PRAGMA temp_store = MEMORY;"));
                db_exec(const_cast<char *>("PRAGMA journal_mode=MEMORY;"));
                db_exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `imagerecord` (`path` VARCHAR NOT NULL , `pHash` BIGINT NOT NULL , PRIMARY KEY (`path`) );"));
                db_exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `filterrecord` (`pHash` BIGINT NOT NULL , `reason` VARCHAR NOT NULL , PRIMARY KEY (`pHash`) );"));
                db_exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `badfilerecord` (`path` VARCHAR NOT NULL , PRIMARY KEY (`path`) );"));
	if(ret) {
		return false;
	}else {
		return true;
	}
}


