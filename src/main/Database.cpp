/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/*
 * Database.cpp
 *
 *  Created on: 21 Aug 2013
 *      Author: nicholas
 */

#include "Database.hpp"
#include "NestedTransaction.hpp"
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

#include <iostream>
#include <iomanip>

using namespace odb;
using namespace imageHasher::db::table;
using imageHasher::db::NestedTransaction;
using namespace boost::log::trivial;

Database::Database(const char* dbPath) {
	std::string new_path(dbPath);
	this->dbName = new_path;
	init();
}

Database::Database(const std::string db_name) {
	this->dbName = db_name;
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

	BOOST_LOG_SEV(logger,info) << "Flushing lists...";

	drainCount = drain();
	drainCount += drain();

	return drainCount;
}

void Database::shutdown() {
	if (running) {
		BOOST_LOG_SEV(logger,info) << "Shutting down...";
		running = false;
		BOOST_LOG_SEV(logger,info) << "Waiting for db worker to finish...";
		workerThread->interrupt();
		workerThread->join();
		delete(workerThread);
		BOOST_LOG_SEV(logger,info) << "Closing database...";
	}
}

void Database::exec(const char* command) {
	orm_db->execute(command);
}

void Database::init() {
	boost::mutex::scoped_lock lock(dbMutex);
	this->currentList = &dataA;
	this->recordsWritten = 0;
	this->invalid_files = 0;
	this->sha_found = 0;
	this->running = true;

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
	this->orm_db.reset(new sqlite::database(dbName,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE));

	BOOST_LOG_SEV(logger,info) << "Setting up database " << dbName;

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

	NestedTransaction t (orm_db->begin());

	for(std::list<db_data>::iterator ite = workList->begin(); ite != workList->end(); ++ite) {
		addToBatch(*ite);
		recordsWritten++;
		drainCount++;
	}

	t.commit();

	workList->clear();
	return drainCount;
}

bool Database::entryExists(fs::path filePath) {
	BOOST_LOG_SEV(logger,debug) <<  "Looking for file " << filePath;

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

	BOOST_LOG_SEV(logger,info) << "Looking for files with path " << directoryPath;

	NestedTransaction t (orm_db->begin());
	std::string *param;

	odb::prepared_query<ImageRecord> pq = prep_query->get_files_with_path_query(param);

	*param = path_query;

	odb::result<ImageRecord> r(pq.execute());

	for(result<ImageRecord>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		filePaths.push_back(fs::path(itr->getPath()));
	}

	t.commit();

	BOOST_LOG_SEV(logger,info) << "Found  " << filePaths.size() << " records for path " << directoryPath;
	return filePaths;
}

void Database::prunePath(std::list<fs::path> filePaths) {
	BOOST_LOG_SEV(logger,info) << "Pruning " << filePaths.size() << " path(s) from the database.";

	for(std::list<fs::path>::iterator ite = filePaths.begin(); ite != filePaths.end(); ++ite){
		ImageRecord ir = get_imagerecord(*ite);

		NestedTransaction t (orm_db->begin());

		if(ir.is_valid()) {
			orm_db->erase(ir);
		}

		t.commit();
	}
}

void Database::addToBatch(db_data data) {
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
		BOOST_LOG_SEV(logger,error) <<  "Unhandled state encountered";
		throw "Unhandled state encountered";
		break;
	}
}

void Database::add_record(db_data data) {
	if(entryExists(data)) {
		BOOST_LOG_SEV(logger,debug) <<  "Entry for " << data.filePath << " already exists, discarding...";
		recordsWritten--;
		return;
	}

	Hash hash = get_hash(data.sha256);

	if (!hash.is_valid()) {
		hash = addHashEntry(data.sha256, data.pHash);
	}else{
		sha_found++;
	}

	try{
	NestedTransaction t(orm_db->begin());

	ImageRecord ir = ImageRecord(data.filePath.string(), &hash);
	orm_db->persist(ir);

	t.commit();
	} catch (const odb::exception &e) {
		BOOST_LOG_SEV(logger,error) <<  "Failed to add ImageRecord for path " << data.filePath << " : " << e.what();
	}
}

int Database::add_path_placeholder(std::string path) {

	NestedTransaction t (orm_db->begin());

	ImageRecord ir (path,NULL);
	int id = orm_db->persist(ir);

	t.commit();

	return id;
}

void Database::add_invalid(db_data data) {
	BOOST_LOG_SEV(logger,debug) <<  "File with path " << data.filePath << " is invalid";
	recordsWritten--;
	invalid_files++;
	Hash hash = get_hash("");
	NestedTransaction t(orm_db->begin());
	ImageRecord ir = ImageRecord(data.filePath.string(), &hash);
	orm_db->persist(ir);

	t.commit();
}

void Database::add_filter(db_data data) {
	BOOST_LOG_SEV(logger,debug) <<  "Adding filter record for pHash" << data.pHash << " with reason " << data.reason;

	imageHasher::db::table::FilterRecord fr(data.pHash, data.reason);

	NestedTransaction t (orm_db->begin());

	orm_db->persist(fr);

	t.commit();
}

void Database::prepareStatements() {
	BOOST_LOG_SEV(logger,info) << "Creating prepared statements...";
	this->prep_query = new imageHasher::db::PreparedQuery(this->orm_db.get());
}

void Database::doWork() {
	while(running) {
		try{
			boost::this_thread::sleep_for(boost::chrono::seconds(3));
		}catch(boost::thread_interrupted&) {
			BOOST_LOG_SEV(logger,info) <<"DB thread interrupted";
		}

		if(currentList->size() > 1000) {
			int drainCount = drain();
			BOOST_LOG_SEV(logger,info) << drainCount << " records processed, Total: " << recordsWritten;
		}
	}

	// make sure both lists are committed
	int drainCount = flush();
	BOOST_LOG_SEV(logger,info) << drainCount << " records processed, Total: " << recordsWritten;
}

unsigned int Database::getRecordsWritten() {
	return recordsWritten;
}

unsigned int Database::get_sha_found() {
	return this->sha_found;
}

unsigned int Database::get_invalid_files() {
	return this->invalid_files;
}

std::string Database::getSHA(fs::path filepath) {
	std::string sha = "";
	BOOST_LOG_SEV(logger,debug) <<  "Getting SHA for path " << filepath;

	ImageRecord ir = get_imagerecord(filepath);

	if(ir.is_valid()) {
		Hash hash = ir.get_hash();
		sha = hash.get_sha256();
	}

	return sha;
}

bool Database::sha_exists(std::string sha) {
	Hash hash = get_hash(sha);
	return hash.is_valid();
}

int64_t Database::getPhash(fs::path filepath) {
	BOOST_LOG_SEV(logger,debug) <<  "Getting pHash for path " << filepath;

	ImageRecord ir = get_imagerecord(filepath);

	if(ir.is_valid()) {
		return ir.get_hash().get_pHash();
	}

	return -1;
}

imageHasher::db::table::Hash Database::get_hash(std::string sha) {
	Hash hash;
	BOOST_LOG_SEV(logger,debug) <<  "Getting hash for sha " << sha;

	NestedTransaction t (orm_db->begin());

	std::string *param;

	odb::prepared_query<Hash> pq = prep_query->get_hash_query(param);

	*param = sha;

	result<Hash> r (pq.execute());

	for(result<Hash>::iterator itr (r.begin()); itr != r.end(); ++itr) {
		hash = *itr;
		break;
	}

	t.commit();

	return hash;
}

imageHasher::db::table::Hash Database::get_hash(u_int64_t phash) {
	Hash hash;
	BOOST_LOG_SEV(logger,debug) <<  "Getting hash for pHash " << phash;

	NestedTransaction t(orm_db->begin());

	uint64_t *param;
	odb::prepared_query<Hash> pq = prep_query->get_hash_query(param);
	*param = phash;

	result<Hash> r(pq.execute());

	for (result<Hash>::iterator itr(r.begin()); itr != r.end(); ++itr) {
		hash = *itr;
		break;
	}

	t.commit();

	return hash;
}

imageHasher::db::table::ImageRecord Database::get_imagerecord(fs::path filepath) {
	ImageRecord ir;

	try {
		NestedTransaction t(orm_db->begin());

		std::string *p;

		BOOST_LOG_SEV(logger,debug) <<  "Getting imagerecord for path " << filepath;
		odb::prepared_query<ImageRecord> pq = this->prep_query->get_imagerecord_path_query(p);

		*p = filepath.string();

		result<ImageRecord> r(pq.execute());

		for (result<ImageRecord>::iterator itr(r.begin()); itr != r.end(); ++itr) {
			ir = *itr;
			break;
		}

		t.commit();
	} catch (odb::exception *e) {
		BOOST_LOG_SEV(logger,error) <<  "Failed to get ImageRecord for path " << filepath << " : " << e;
	}

	return ir;
}

Hash Database::addHashEntry(std::string sha, u_int64_t pHash) {
	Hash hash(sha,pHash);
	NestedTransaction t (orm_db->begin());
		orm_db->persist(hash);
	t.commit();

	return hash;
}

int Database::prune_hash_table() {
	//TODO Replace with ODB ORM queries

	NestedTransaction t (orm_db->begin());

	BOOST_LOG_SEV(logger,info) << "Pruning hash table...";
	unsigned long long query_result = orm_db->execute(prune_hash_table_query);

	t.commit();

	query_result -= 1;

	BOOST_LOG_SEV(logger,info) << "Pruned " << query_result << " record(s) from the hash table.";

	return (int)query_result;
}
