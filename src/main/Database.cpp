/*
 * Database.cpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#include "../include/Database.hpp"

const char *dbName = "imageHasher.db";

const char *insertImageQuery = "INSERT INTO `imagerecord` (`path`,`pHash`, `sha256`) VALUES (?,?,?);";
const char *insertInvalidQuery = "INSERT INTO `badfilerecord` (`path`) VALUES (?);";
const char *insertFilterQuery = "INSERT OR IGNORE INTO `filterrecord` (`pHash`, `reason`) VALUES (?,?);";
const char *prunePathQuery = "SELECT `path` FROM `imagerecord` WHERE `path` LIKE ? UNION SELECT `path` FROM `badfilerecord` WHERE `path` LIKE ?;";
const char *prunePathDeleteImage = "DELETE FROM `imagerecord` WHERE `path` = ?;";
const char *prunePathDeleteBadFile = "DELETE FROM `badfilerecord` WHERE `path` = ?;";
const char *checkExistsQuery = "SELECT EXISTS(SELECT 1 FROM `imagerecord` WHERE `path` = ? LIMIT 1) OR EXISTS(SELECT 1 FROM `badfilerecord`  WHERE `path` = ?  LIMIT 1);";
const char *checkSHAQuery = "SELECT EXISTS(SELECT 1 FROM `imagerecord` WHERE `path` = ? AND `sha256` != '' LIMIT 1);";
const char *updateSha = "UPDATE `imagerecord` SET sha256=? WHERE `path`=?";
const char *getSHAQuery = "SELECT `sha256` FROM `imagerecord` WHERE `path` = ?;";

const char *startTransactionQuery = "BEGIN TRANSACTION;";
const char *commitTransactionQuery = "COMMIT TRANSACTION;";

static const int CURRENT_DB_SCHEMA_VERSION = 1;

Database::Database(const char* dbPath) {
	dbName = dbPath;
	init();
}

Database::Database() {
	init();
}

Database::~Database() {
	shutdown();
}

int Database::getCurrentSchemaVersion() {
	return CURRENT_DB_SCHEMA_VERSION;
}

int Database::flush() {
	int drainCount = 0;

	LOG4CPLUS_INFO(logger, "Flushing lists...");

	drainCount = drain();
	drainCount += drain();

	return drainCount;
}

void Database::shutdown() {
	if (running) {
		LOG4CPLUS_INFO(logger, "Shutting down...");
		running = false;
		LOG4CPLUS_INFO(logger, "Waiting for db worker to finish...");
		workerThread->interrupt();
		workerThread->join();
		LOG4CPLUS_INFO(logger, "Closing database...");
		sqlite3_close(db);
	}
}

void Database::exec(const char* command) {
	sqlite3_exec(db, command, NULL, NULL, &errMsg);

	if(errMsg != NULL) {
		LOG4CPLUS_WARN(logger, "Exec failed: " << command << " -> " << errMsg);
		sqlite3_free(errMsg);
	}
}

void Database::init() {
	boost::mutex::scoped_lock lock(dbMutex);
	this->currentList = &dataA;
	this->recordsWritten = 0;
	this->running = true;
	logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Database"));

	setupDatabase();
	prepareStatements();

	workerThread = new boost::thread(&Database::doWork, this);
}

void Database::setupDatabase() {
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

                updateSchema();
	if(ret) {
		LOG4CPLUS_ERROR(logger, "Database setup failed");
		throw "Database setup failed";
	}
}

void Database::updateSchema() {
	int dbVersion = getUserSchemaVersion();

	if(dbVersion == CURRENT_DB_SCHEMA_VERSION){
		LOG4CPLUS_INFO_STR(logger,"DB schema is up to date.");
	}else if(dbVersion > CURRENT_DB_SCHEMA_VERSION) {
		LOG4CPLUS_INFO(logger,"DB schema is newer than the current schema, aborting...");
		throw std::runtime_error("DB schema is newer than the current version");
	}else{
		LOG4CPLUS_INFO(logger,"DB schema is out of date, actual: " << dbVersion << " current: " << CURRENT_DB_SCHEMA_VERSION << " updating...");
	switch (dbVersion) {
		case 0:
			exec(const_cast<char *>("ALTER TABLE imagerecord ADD COLUMN `sha256` VARCHAR NOT NULL DEFAULT ''"));
			break;

		default:
			break;
	}

	setUserSchemaVersion(CURRENT_DB_SCHEMA_VERSION);
	}
}

void Database::updateSHA256(std::string path, std::string sha){
	sqlite3_bind_text(updateShaStmt, 1, sha.c_str(), sha.size(), SQLITE_STATIC);
	sqlite3_bind_text(updateShaStmt, 2, path.c_str(), path.size(), SQLITE_STATIC);

	sqlite3_step(updateShaStmt);

	sqlite3_reset(updateShaStmt);
}

void Database::add(db_data data) {
	boost::mutex::scoped_lock lock(flipMutex);
	currentList->push_back(data);
}

void Database::flipLists() {
	boost::mutex::scoped_lock lock(flipMutex);

	if(currentList == &dataA) {
		currentList = &dataB;
	}else{
		currentList = &dataA;
	}
}

int Database::drain() {
	boost::mutex::scoped_lock lock(dbMutex);
	std::list<db_data>* workList;
	int drainCount = 0;

	workList = currentList;
	flipLists();

	startTransaction();

	for(std::list<db_data>::iterator ite = workList->begin(); ite != workList->end(); ++ite) {
		addToBatch(*ite);
		recordsWritten++;
		drainCount++;
	}

	commitTransaction();

	workList->clear();
	return drainCount;
}

bool Database::entryExists(fs::path filePath) {
boost::mutex::scoped_lock lock(dbMutex);
	const char* path = filePath.c_str();
	int pathSize = filePath.string().size();

	LOG4CPLUS_DEBUG(logger, "Looking for file " << path);

	sqlite3_bind_text(checkExistsStmt, 1, path, pathSize, SQLITE_STATIC);
	sqlite3_bind_text(checkExistsStmt, 2, path, pathSize, SQLITE_STATIC);

	sqlite3_step(checkExistsStmt);
	int response = sqlite3_column_int(checkExistsStmt, 0);
	sqlite3_reset(checkExistsStmt);

	if (response == 1) {
		return true;
	} else {
		return false;
	}
}

bool Database::entryExists(db_data data) {
	return entryExists(data.filePath);
}

bool Database::hasSHA(fs::path filepath) {
	boost::mutex::scoped_lock lock(dbMutex);

	LOG4CPLUS_DEBUG(logger, "Looking if " << filepath << " has SHA");

	sqlite3_bind_text(checkSHAStmt, 1, filepath.c_str(), filepath.string().size(), SQLITE_STATIC);


	sqlite3_step(checkSHAStmt);
		int response = sqlite3_column_int(checkSHAStmt, 0);
		sqlite3_reset(checkSHAStmt);

		if (response == 1) {
			return true;
		} else {
			return false;
		}

	return false;
}

std::list<fs::path> Database::getFilesWithPath(fs::path directoryPath) {
	std::list<fs::path> filePaths;
	std::string query(directoryPath.string());
	query += "%";
	const char* path = query.c_str();
	int pathSize = query.size();
	int response = -1;

	LOG4CPLUS_INFO(logger, "Looking for files with path " << directoryPath);

	boost::mutex::scoped_lock lock(dbMutex);
	sqlite3_bind_text(pruneQueryStmt, 1, path, pathSize, SQLITE_STATIC );
	sqlite3_bind_text(pruneQueryStmt, 2, path, pathSize, SQLITE_STATIC );

	response = sqlite3_step(pruneQueryStmt);

	while (SQLITE_ROW == response) {
		std::string resultPath;
		resultPath.append(reinterpret_cast<const char*>(sqlite3_column_text(pruneQueryStmt, 0)));
		fs::path p(resultPath);
		filePaths.push_back(p);

		response = sqlite3_step(pruneQueryStmt);
	}

	sqlite3_reset(pruneQueryStmt);

	LOG4CPLUS_INFO(logger, "Found  " << filePaths.size() << " records for path " << directoryPath);
	return filePaths;
}

void Database::startTransaction() {
	int response = sqlite3_step(startTrStmt);
	sqlite3_reset(startTrStmt);
	if (response != SQLITE_DONE) {
		LOG4CPLUS_WARN(logger,
				"Failed to start transaction: " << sqlite3_errmsg(db));
	}
}

void Database::commitTransaction() {
	int response = sqlite3_step(commitTrStmt);
	sqlite3_reset(commitTrStmt);
	if (response != SQLITE_DONE) {
		LOG4CPLUS_WARN(logger,
				"Failed to commit transaction: " << sqlite3_errmsg(db));
	}
}

void Database::prunePath(std::list<fs::path> filePaths) {
	boost::mutex::scoped_lock lock(dbMutex);

	bool allOk = true;

	startTransaction();

	for(std::list<fs::path>::iterator ite = filePaths.begin(); ite != filePaths.end(); ++ite){
		const char* path = ite->c_str();
		int pathSize = ite->string().size();
		int response = 0;

		sqlite3_bind_text(pruneDeleteImageStmt, 1, path, pathSize, SQLITE_TRANSIENT);
		sqlite3_bind_text(pruneDeleteBadFileStmt, 1, path, pathSize, SQLITE_TRANSIENT);

		response = sqlite3_step(pruneDeleteImageStmt);
		if(SQLITE_DONE != response) {allOk = false;}
		response = sqlite3_step(pruneDeleteBadFileStmt);
		if(SQLITE_DONE != response) {allOk = false;}

		sqlite3_reset(pruneDeleteImageStmt);
		sqlite3_reset(pruneDeleteBadFileStmt);
	}

	commitTransaction();

	if (!allOk) {
		LOG4CPLUS_WARN(logger, "Failed to delete some file paths");
	}
}

void Database::addToBatch(db_data data) {
	int response = 0;

	switch (data.status) {
	case OK:
		sqlite3_bind_text(addOkStmt, 1, data.filePath.c_str(), data.filePath.string().size(), SQLITE_STATIC );
		sqlite3_bind_int64(addOkStmt, 2, data.pHash);
		sqlite3_bind_text(addOkStmt, 3, data.sha256.c_str(), data.sha256.size(), SQLITE_STATIC);

		response = sqlite3_step(addOkStmt);

		if (response != SQLITE_DONE) {
			LOG4CPLUS_WARN(logger, "Failed to add " << data.filePath << " / " << data.pHash);
			recordsWritten--;
		}
		sqlite3_reset(addOkStmt);
		break;

	case SHA:
		updateSHA256(data.filePath.string(), data.sha256);
		break;

	case INVALID:
		sqlite3_bind_text(addInvalidStmt, 1, data.filePath.c_str(), data.filePath.string().size(), SQLITE_STATIC );
		response = sqlite3_step(addInvalidStmt);

		if (response != SQLITE_DONE) {
			LOG4CPLUS_WARN(logger, "Failed to add " << data.filePath);
			recordsWritten--;
		}

		sqlite3_reset(addInvalidStmt);
		break;

	case FILTER:
		sqlite3_bind_int64(addFilterStmt, 1, data.pHash);
		sqlite3_bind_text(addFilterStmt, 2, data.reason.c_str(), data.reason.size(), SQLITE_STATIC);
		response = sqlite3_step(addFilterStmt);

		if (response != SQLITE_DONE) {
			LOG4CPLUS_WARN(logger, "Failed to add filter for " << data.pHash << " " << data.reason);
		}

		sqlite3_reset(addFilterStmt);
		break;

	default:
		LOG4CPLUS_ERROR(logger, "Unhandled state encountered");
		throw "Unhandled state encountered";
		break;
	}
}

void Database::prepareStatements() {
	LOG4CPLUS_INFO(logger, "Creating prepared statements...");

	createPreparedStatement(insertImageQuery, addOkStmt);
	createPreparedStatement(insertInvalidQuery, addInvalidStmt);
	createPreparedStatement(insertFilterQuery,addFilterStmt);
	createPreparedStatement(checkExistsQuery, checkExistsStmt);
	createPreparedStatement(checkSHAQuery,checkSHAStmt);
	createPreparedStatement(getSHAQuery, getSHAqueryStmt);

	createPreparedStatement(startTransactionQuery, startTrStmt);
	createPreparedStatement(commitTransactionQuery, commitTrStmt);

	createPreparedStatement(prunePathQuery, pruneQueryStmt);
	createPreparedStatement(prunePathDeleteImage, pruneDeleteImageStmt);
	createPreparedStatement(prunePathDeleteBadFile, pruneDeleteBadFileStmt);

	createPreparedStatement(updateSha, updateShaStmt);
}

void Database::createPreparedStatement(const char *&query, sqlite3_stmt *&stmt) {
	int error = 0;

	error = sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);

	if (SQLITE_OK != error) {
		LOG4CPLUS_ERROR(logger, "Failed to prepare statement " << query << " -> " << error);
		throw "Prepare statement failed";
	}
}

void Database::doWork() {
	while(running) {
		try{
			boost::this_thread::sleep_for(boost::chrono::seconds(3));
		}catch(boost::thread_interrupted&) {
			LOG4CPLUS_INFO(logger,"DB thread interrupted");
		}

		if(currentList->size() > 1000) {
			int drainCount = drain();
			LOG4CPLUS_INFO(logger, drainCount << " records processed, Total: " << recordsWritten);
		}
	}

	// make sure both lists are committed
	int drainCount = flush();
	LOG4CPLUS_INFO(logger, drainCount << " records processed, Total: " << recordsWritten);
}

unsigned int Database::getRecordsWritten() {
	return recordsWritten;
}

std::string Database::getSHA(fs::path filepath) {
	LOG4CPLUS_DEBUG(logger, "Getting SHA for path " << filepath);

	boost::mutex::scoped_lock lock(dbMutex);
	sqlite3_bind_text(getSHAqueryStmt, 1, filepath.c_str(), filepath.string().size(), SQLITE_STATIC );

	int response = sqlite3_step(getSHAqueryStmt);
	std::string sha;

	if (SQLITE_ROW == response) {
		std::string resultPath;
		sha = (reinterpret_cast<const char*>(sqlite3_column_text(getSHAqueryStmt, 0)));
		LOG4CPLUS_DEBUG(logger, "Found SHA " << sha << " for file " << filepath);
	}

	sqlite3_reset(getSHAqueryStmt);

	return sha;
}

int userVersionCallback(void* dbVersion,int numOfresults,char** valuesAsString,char** columnNames) {
	int version = -1;

	if(numOfresults == 1) {
		version = atoi(valuesAsString[0]);
		*((int*)dbVersion) = version;
	}

	return 0;
}

int Database::getUserSchemaVersion() {
	int dbVersion = -1;
	sqlite3_exec(db, "PRAGMA user_version;", userVersionCallback, &dbVersion, &errMsg);

	if(errMsg != NULL) {
		LOG4CPLUS_WARN(logger, "Failed to get user version" << " -> " << errMsg);
		sqlite3_free(errMsg);
	}

	return dbVersion;
}

void Database::setUserSchemaVersion(int version) {
	std::stringstream command;
	command << "PRAGMA user_version = ";
	command << version;
	command << ";";

	Database::exec(command.str().c_str());
}

