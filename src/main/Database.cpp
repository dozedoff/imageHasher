/*
 * Database.cpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#include "../include/Database.hpp"
#include <string>

#include <odb/transaction.hxx>
#include <odb/result.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/exceptions.hxx>
#include <odb/exceptions.hxx>

#include "table/ImageRecord.hpp"
#include "table/ImageRecord-odb.hxx"

#include "table/FilterRecord.hpp"
#include "table/FilterRecord-odb.hxx"

const char *dbName = "imageHasher.db";

const char *insertImageQuery = "INSERT INTO `imagerecord` (`path`,`sha_id`,`phash_id`) VALUES (?,?,?);";
const char *insertInvalidQuery = "INSERT INTO `badfilerecord` (`path`) VALUES (?);";
const char *insertFilterQuery = "INSERT OR IGNORE INTO `filterrecord` (`phash_id`, `reason`) VALUES (?,?);";
const char *prunePathQuery = "SELECT `path` FROM `imagerecord` WHERE `path` LIKE ? UNION SELECT `path` FROM `badfilerecord` WHERE `path` LIKE ?;";
const char *prunePathDeleteImage = "DELETE FROM `imagerecord` WHERE `path` = ?;";
const char *prunePathDeleteBadFile = "DELETE FROM `badfilerecord` WHERE `path` = ?;";
const char *checkExistsQuery = "SELECT EXISTS(SELECT 1 FROM `imagerecord` WHERE `path` = ? LIMIT 1) OR EXISTS(SELECT 1 FROM `badfilerecord`  WHERE `path` = ?  LIMIT 1);";
const char *checkSHAQuery = "SELECT EXISTS(SELECT 1 FROM `imagerecord` JOIN `sha_hash` ON `sha_id`= `id` WHERE `path` = ? AND `sha256` != '' LIMIT 1);";
const char *updateSha = "UPDATE `imagerecord` SET `sha_id`=? WHERE `path` = ?;";
const char *getSHAQuery = "SELECT `sha256` FROM `imagerecord` AS ir JOIN `sha_hash` AS h ON ir.sha_id=h.id WHERE `path` = ?;";
const char *getPhashQuery = "SELECT `pHash` FROM `imagerecord` AS ir JOIN `phash_hash` AS h ON ir.sha_id=h.id WHERE `path` = ?;";
const char *getSHAidQuery = "SELECT `id` FROM `sha_hash` WHERE `sha256`= ?;";
const char *getpHashidQuery = "SELECT `id` FROM `phash_hash` WHERE `pHash`= ?;";
const char *insertShaRecordQuery = "INSERT INTO `sha_hash` (`sha256`) VALUES (?);";
const char *insertpHashRecordQuery = "INSERT INTO `phash_hash` (`pHash`) VALUES (?);";

const char *startTransactionQuery = "BEGIN TRANSACTION;";
const char *commitTransactionQuery = "COMMIT TRANSACTION;";

using namespace odb;
using namespace imageHasher::db::table;

Database::Database(const char* dbPath) {
	dbName = dbPath;
	init();
}

Database::Database() {
	init();
}

Database::~Database() {
	shutdown();
	delete this->prep_query;
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
	}
}

void Database::exec(const char* command) {
	orm_db->execute(command);
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

bool Database::is_db_initialised() {
	Hash hash;
	bool exists = true;
	transaction t(orm_db->begin());
	try {
		exists = orm_db->find(1, hash);
	} catch (odb::sqlite::database_exception& e) {
		exists = false;
	}
	t.commit();
	return exists;
}

void Database::initialise_db() {
	transaction t(orm_db->begin());
	odb::schema_catalog::create_schema(*orm_db, "", false);
	t.commit();
	addHashEntry("", 0);
}

void Database::setupDatabase() {
	orm_db = new sqlite::database(dbName,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

	LOG4CPLUS_INFO(logger, "Setting up database " << dbName);

	bool init_done = is_db_initialised();

	if(! init_done) {
		initialise_db();
	}

	transaction t_pragma (orm_db->begin());
		orm_db->execute("PRAGMA page_size = 4096;");
		orm_db->execute("PRAGMA cache_size=10000;");
		orm_db->execute("PRAGMA locking_mode=EXCLUSIVE;");
		orm_db->execute("PRAGMA temp_store = MEMORY;");
		orm_db->execute("PRAGMA journal_mode=MEMORY;");
	t_pragma.commit();

	// perform out of transaction execution
	connection_ptr c (orm_db->connection());
	c->execute("PRAGMA synchronous=NORMAL;");
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

	for(std::list<db_data>::iterator ite = workList->begin(); ite != workList->end(); ++ite) {
		addToBatch(*ite);
		recordsWritten++;
		drainCount++;
	}

	workList->clear();
	return drainCount;
}

bool Database::entryExists(fs::path filePath) {
	LOG4CPLUS_DEBUG(logger, "Looking for file " << filePath);

	ImageRecord ir = get_imagerecord(filePath);

	return ir.is_valid();
}

bool Database::entryExists(db_data data) {
	return entryExists(data.filePath);
}

std::list<fs::path> Database::getFilesWithPath(fs::path directoryPath) {
	std::list<fs::path> filePaths;
	std::string path_query(directoryPath.string());
	path_query += "%";

	LOG4CPLUS_INFO(logger, "Looking for files with path " << directoryPath);

	transaction t (orm_db->begin());
	result<ImageRecord> r (orm_db->query<ImageRecord>(query<ImageRecord>::path.like(path_query)));

	for(result<ImageRecord>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		filePaths.push_back(fs::path(itr->getPath()));
	}

	t.commit();

	LOG4CPLUS_INFO(logger, "Found  " << filePaths.size() << " records for path " << directoryPath);
	return filePaths;
}

void Database::prunePath(std::list<fs::path> filePaths) {
	for(std::list<fs::path>::iterator ite = filePaths.begin(); ite != filePaths.end(); ++ite){
		ImageRecord ir = get_imagerecord(*ite);

		transaction t (orm_db->begin());

		if(ir.is_valid()) {
			orm_db->erase(ir);
		}

		t.commit();
	}
}

void Database::addToBatch(db_data data) {
	int response = 0;
	int hashId = -1;

	switch (data.status) {
	case OK:
		add_record(data);
		break;

	case INVALID:
		add_invalid(data);
		break;

	case FILTER:
		add_filter(data);
		break;

	default:
		LOG4CPLUS_ERROR(logger, "Unhandled state encountered");
		throw "Unhandled state encountered";
		break;
	}
}

void Database::add_record(db_data data) {
	Hash hash = get_hash(data.sha256);

	if (!hash.is_valid()) {
		hash = addHashEntry(data.sha256, data.pHash);
	}

	if(entryExists(data)) {
		LOG4CPLUS_DEBUG(logger, "Entry for " << data.filePath << " already exists, discarding...");
		recordsWritten--;
		return;
	}

	try{
	transaction t(orm_db->begin());

	ImageRecord ir = ImageRecord(data.filePath.string(), &hash);
	orm_db->persist(ir);

	t.commit();
	} catch (const odb::exception &e) {
		LOG4CPLUS_ERROR(logger, "Failed to add ImageRecord for path " << data.filePath << " : " << e.what());
	}
}

int Database::add_path_placeholder(std::string path) {

	transaction t (orm_db->begin());

	ImageRecord ir (path,NULL);
	int id = orm_db->persist(ir);

	t.commit();

	return id;
}

void Database::add_invalid(db_data data) {
	LOG4CPLUS_DEBUG(logger, "File with path " << data.filePath << " is invalid");
	recordsWritten--;
	Hash hash = get_hash("");
	transaction t(orm_db->begin());
	ImageRecord ir = ImageRecord(data.filePath.string(), &hash);
	orm_db->persist(ir);

	t.commit();
}

void Database::add_filter(db_data data) {
	LOG4CPLUS_DEBUG(logger, "Adding filter record for pHash" << data.pHash << " with reason " << data.reason);

	imageHasher::db::table::FilterRecord fr(data.pHash, data.reason);

	transaction t (orm_db->begin());

	orm_db->persist(fr);

	t.commit();
}

void Database::prepareStatements() {
	LOG4CPLUS_INFO(logger, "Creating prepared statements...");
	this->prep_query = new imageHasher::db::PreparedQuery(this->orm_db);
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
	std::string sha = "";
	LOG4CPLUS_DEBUG(logger, "Getting SHA for path " << filepath);

	transaction t (orm_db->begin());

	result<ImageRecord> r (orm_db->query<ImageRecord>(query<ImageRecord>::path == filepath.string()));

	for(result<ImageRecord>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		Hash hash = itr->get_hash();
		sha = hash.get_sha256();
		break;
	}

	t.commit();

	return sha;
}

bool Database::sha_exists(std::string sha) {
	Hash hash = get_hash(sha);
	return hash.is_valid();
}

int64_t Database::getPhash(fs::path filepath) {
	LOG4CPLUS_DEBUG(logger, "Getting pHash for path " << filepath);

	ImageRecord ir = get_imagerecord(filepath);

	if(ir.is_valid()) {
		return ir.get_hash().get_pHash();
	}

	return -1;
}

imageHasher::db::table::Hash Database::get_hash(std::string sha) {
	Hash hash;
	LOG4CPLUS_DEBUG(logger, "Getting hash for sha " << sha);

	transaction t (orm_db->begin());

	result<Hash> r (orm_db->query<Hash>(query<Hash>::sha256 == sha));

	for(result<Hash>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		hash = *itr;
		break;
	}

	t.commit();

	return hash;
}

imageHasher::db::table::Hash Database::get_hash(u_int64_t phash) {
	Hash hash;
	LOG4CPLUS_DEBUG(logger, "Getting hash for pHash " << phash);

	transaction t (orm_db->begin());

	result<Hash> r (orm_db->query<Hash>(query<Hash>::pHash == phash));

	for(result<Hash>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		hash = *itr;
		break;
	}

	t.commit();

	return hash;
}

imageHasher::db::table::ImageRecord Database::get_imagerecord(fs::path filepath) {
	ImageRecord ir;

	try {
		transaction t(orm_db->begin());

		std::string *p;

		LOG4CPLUS_DEBUG(logger, "Getting imagerecord for path " << filepath);
		odb::prepared_query<ImageRecord> pq = this->prep_query->get_imagerecord_path_query(p);

		p->append(filepath.string());

		result<ImageRecord> r(pq.execute());

		for (result<ImageRecord>::iterator itr(r.begin()); itr != r.end(); ++itr) {
			ir = *itr;
			break;
		}

		t.commit();
	} catch (odb::exception *e) {
		LOG4CPLUS_ERROR(logger, "Failed to get ImageRecord for path " << filepath << " : " << e);
	}

	return ir;
}

Hash Database::addHashEntry(std::string sha, u_int64_t pHash) {
	Hash hash(sha,pHash);
	transaction t (orm_db->begin());
		orm_db->persist(hash);
	t.commit();

	return hash;
}
