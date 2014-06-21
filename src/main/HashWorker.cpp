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
 * HashWorker.cpp
 *
 *  Created on: 20 Aug 2013
 *      Author: nicholas
 */

#include "../include/HashWorker.hpp"
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
	LOG4CPLUS_INFO(logger, "Hashed " << db.getRecordsWritten() << " of " << totalNumOfFiles << " images");
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

	while (running) {
		path image = getWork();

		if (image.empty() || !boost::filesystem::exists(image)) {
			LOG4CPLUS_DEBUG(logger, "HashWorker " << workerNum << ": " << "Path empty or invalid, skipping... " << image);
			continue;
		}

		try {
			filepath = image.string();
			data = Database::db_data(image);

			if (db.entryExists(data)) {
				LOG4CPLUS_DEBUG(logger, "HashWorker " << workerNum << ": " << "Found " << image << " skipping...");
				continue;
			}

			pHash = iph.getLongHash(filepath);
			sha256 = sha.sha256(filepath);

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

	LOG4CPLUS_INFO(logger, "HashWorker " << workerNum << ": " << "No more work, worker thread shutting down");
}
