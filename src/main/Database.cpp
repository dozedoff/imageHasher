/*
 * Database.cpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#include "../include/Database.hpp"

const char *dbName = "imageHasher.db";

const char *insertImageQuery = "INSERT INTO `imagerecord` (`path`,`pHash`) VALUES (?,?)";
const char *insertInvalidQuery = "INSERT INTO `badfilerecord` (`path`) VALUES (?);";
const char *checkExistsQuery = "SELECT EXISTS(SELECT 1 FROM `imagerecord` WHERE `path` = ? LIMIT 1) OR EXISTS(SELECT 1 FROM `badfilerecord`  WHERE `path` = ?  LIMIT 1);";

const char *startTransactionQuery = "BEGIN TRANSACTION;";
const char *commitTransactionQuery = "COMMIT TRANSACTION;";

Database::Database() {
	this->currentList = &dataA;
	this->recordsWritten = 0;
	this->running = true;
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Database"));

	init();
	prepareStatements();
	workerThread = new boost::thread(&Database::doWork, this);
}

Database::~Database() {
	shutdown();
}

void Database::shutdown() {
	LOG4CPLUS_INFO(logger, "Shutting down...");
	running = false;
	LOG4CPLUS_INFO(logger, "Waiting for db worker to finish...");
	workerThread->join();
	LOG4CPLUS_INFO(logger, "Closing database...");
	sqlite3_close_v2(db);
}

void Database::exec(const char* command) {
	sqlite3_exec(db, command, NULL, NULL, &errMsg);

	if(errMsg != NULL) {
		LOG4CPLUS_WARN(logger, "Exec failed: " << command << " -> " << errMsg);
		sqlite3_free(errMsg);
	}
}

void Database::init() {
	LOG4CPLUS_INFO(logger, "Setting up database " << dbName);
	int ret = sqlite3_open(dbName, &db);
                exec(const_cast<char *>("PRAGMA page_size = 4096;"));
                exec(const_cast<char *>("PRAGMA cache_size=10000;"));
                exec(const_cast<char *>("PRAGMA locking_mode=EXCLUSIVE;"));
                exec(const_cast<char *>("PRAGMA synchronous=NORMAL;"));
                exec(const_cast<char *>("PRAGMA temp_store = MEMORY;"));
                exec(const_cast<char *>("PRAGMA journal_mode=MEMORY;"));
                exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `imagerecord` (`path` VARCHAR NOT NULL , `pHash` BIGINT NOT NULL , PRIMARY KEY (`path`) );"));
                exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `filterrecord` (`pHash` BIGINT NOT NULL , `reason` VARCHAR NOT NULL , PRIMARY KEY (`pHash`) );"));
                exec(const_cast<char *>("CREATE TABLE IF NOT EXISTS `badfilerecord` (`path` VARCHAR NOT NULL , PRIMARY KEY (`path`) );"));
	if(ret) {
		LOG4CPLUS_ERROR(logger, "Database setup failed");
		throw "Database setup failed";
	}
}

void Database::add(db_data data) {
	boost::mutex::scoped_lock(flipMutex);
	currentList->push_back(data);
}

void Database::flipLists() {
	boost::mutex::scoped_lock(flipMutex);

	if(currentList == &dataA) {
		currentList = &dataB;
	}else{
		currentList = &dataA;
	}
}

void Database::drain() {
	std::list<db_data>* workList;

	workList = currentList;
	flipLists();

	int response = 0;
	response = sqlite3_step(startTrStmt);
	sqlite3_reset(startTrStmt);

	if(response != SQLITE_DONE) {
		LOG4CPLUS_WARN(logger, "Failed to start transaction: " << sqlite3_errstr(response));
	}


	for(std::list<db_data>::iterator ite = workList->begin(); ite != workList->end(); ++ite) {
		addToBatch(*ite);
		recordsWritten++;
	}

	response = sqlite3_step(commitTrStmt);
	sqlite3_reset(commitTrStmt);

	if(response != SQLITE_DONE) {
		LOG4CPLUS_WARN(logger, "Failed to commit transaction: " << sqlite3_errstr(response));
	}

	workList->clear();
}

void Database::addToBatch(db_data data) {
	int response = 0;

	switch (data.status) {
	case OK:
		sqlite3_bind_text(addOkStmt, 1, data.filePath.c_str(), data.filePath.string().size(), SQLITE_STATIC );
		sqlite3_bind_int64(addOkStmt, 2, data.pHash);
		response = sqlite3_step(addOkStmt);

		if (response != SQLITE_DONE) {
			LOG4CPLUS_WARN(logger, "Failed to add " << data.filePath << " / " << data.pHash);
		}
		sqlite3_reset(addOkStmt);
		break;

	case INVALID:
		sqlite3_bind_text(addInvalidStmt, 1, data.filePath.c_str(), data.filePath.string().size(), SQLITE_STATIC );
		response = sqlite3_step(addInvalidStmt);

		if (response != SQLITE_DONE) {
			LOG4CPLUS_WARN(logger, "Failed to add " << data.filePath);
		}

		sqlite3_reset(addInvalidStmt);
		break;

	default:
		LOG4CPLUS_ERROR(logger, "Unhandled state encountered");
		throw "Unhandled state encountered";
		break;
	}
}

void Database::prepareStatements() {
	LOG4CPLUS_INFO(logger, "Creating prepared statements...");
	sqlite3_prepare_v2(db, insertImageQuery, strlen(insertImageQuery), &addOkStmt, NULL);
	sqlite3_prepare_v2(db, insertInvalidQuery, strlen(insertInvalidQuery), &addInvalidStmt, NULL);
	sqlite3_prepare_v2(db, checkExistsQuery, strlen(checkExistsQuery), &checkExistsStmt, NULL);

	sqlite3_prepare_v2(db, startTransactionQuery, strlen(startTransactionQuery), &startTrStmt, NULL);
	sqlite3_prepare_v2(db, commitTransactionQuery, strlen(commitTransactionQuery), &commitTrStmt, NULL);
}

void Database::doWork() {
	while(running) {
		boost::this_thread::sleep_for(boost::chrono::seconds(3));

		if(currentList->size() > 10) {
			drain();
		}

		LOG4CPLUS_INFO(logger, recordsWritten << " records processed");
	}

	// make sure both lists are committed
	drain();
	drain();
}

unsigned int Database::getRecordsWritten() {
	return recordsWritten;
}
