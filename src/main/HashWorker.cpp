/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/*
 * HashWorker.cpp
 *
 *  Created on: 20 Aug 2013
 *      Author: nicholas
 */

#include "../include/HashWorker.hpp"
#include "table/Hash.hpp"
#include "../../commoncpp/src/include/commoncpp.hpp"
#include "../../commoncpp/src/include/hash/SHA.hpp"

#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/Magick++/Exception.h>

using std::list;

HashWorker::HashWorker(list<path> *imagePaths,int numOfWorkers = 1) : numOfWorkers(numOfWorkers), imagePaths(*imagePaths) {
	running = true;
	if (numOfWorkers > 0) {

	} else {
		throw "Number of  workers must be greater than 0";
	}

	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashWorker"));
	totalNumOfFiles = imagePaths->size();
	skipped_files = 0;
}

void HashWorker::start() {
	boost::thread_group tg;

	LOG4CPLUS_INFO(logger, "Starting " << numOfWorkers << " worker thread(s)");

	for(int i = 0; i < numOfWorkers; i++) {
		boost::thread *t = new boost::thread(&HashWorker::doWork, this, i);
		tg.add_thread(t);
		LOG4CPLUS_INFO(logger, "Worker thread " << i << " started");
	}

	tg.join_all();
	db.shutdown();
	LOG4CPLUS_INFO(logger, "All worker thread(s) have terminated");
	LOG4CPLUS_INFO(logger, "Hashed " << db.getRecordsWritten() << " of " << totalNumOfFiles << " images, sha lookup hits:"<< db.get_sha_found() << ", skipped " << skipped_files << ", invalid: " << db.get_invalid_files());
}

void HashWorker::clear() {
	boost::mutex::scoped_lock lock(workQueueMutex);
	imagePaths.clear();
}

path HashWorker::getWork() {
	boost::mutex::scoped_lock lock(workQueueMutex);

	if (!imagePaths.empty()) {
		path next = imagePaths.front();
		imagePaths.pop_front();
		return next;
	} else {
		running = false;
		return boost::filesystem::path();
	}
}

void HashWorker::doWork(int workerNum) {
	ImagePHash iph(32, 9);
	SHA sha;
	int64_t pHash = 0;
	std::string sha256 = "";
	std::string filepath;
	Database::db_data data;

	unsigned int local_skipped_files = 0;

	while (running) {
		path image = getWork();

		if (image.empty() || !boost::filesystem::exists(image)) {
			LOG4CPLUS_DEBUG(logger, "HashWorker " << workerNum << ": " << "Path empty or invalid, skipping... " << image);
			local_skipped_files++;
			continue;
		}

		try {
			filepath = image.string();
			data = Database::db_data(image);

			if (db.entryExists(data)) {
				LOG4CPLUS_DEBUG(logger, "HashWorker " << workerNum << ": " << "Found " << image << " skipping...");
				local_skipped_files++;
				continue;
			}

			sha256 = sha.sha256(filepath);

			if(!db.sha_exists(sha256)) {
				pHash = iph.getLongHash(filepath);
			}else{
				imageHasher::db::table::Hash hash = db.get_hash(sha256);
				pHash = hash.get_pHash();
			}

			data.pHash = pHash;
			data.sha256 = sha256;
			data.status = Database::OK;
			LOG4CPLUS_DEBUG(logger, "HashWorker " << workerNum << ": " << pHash << " - " << image);
			db.add(data);
		} catch (Magick::Exception &e) {
			LOG4CPLUS_WARN(logger, "HashWorker " << workerNum << ": " << "Failed to process image " << filepath << " : " << e.what());
			data.status = Database::INVALID;
			db.add(data);
		}
	}

	boost::mutex::scoped_lock lock(stats_mutex);
	skipped_files += local_skipped_files;

	LOG4CPLUS_INFO(logger, "HashWorker " << workerNum << ": " << "No more work, worker thread shutting down");
}
