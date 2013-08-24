/*
 * HashWorker.cpp
 *
 *  Created on: 20 Aug 2013
 *      Author: nicholas
 */

#include "../include/HashWorker.hpp"
#include "../../commoncpp/src/include/commoncpp.hpp"

#include <GraphicsMagick/Magick++.h>
#include <GraphicsMagick/Magick++/Exception.h>

using std::list;

HashWorker::HashWorker(list<path> *imagePaths,int numOfWorkers = 1) : numOfWorkers(numOfWorkers), imagePaths(*imagePaths) {
	if (numOfWorkers > 0) {

	} else {
		throw "Number of  workers must be greater than 0";
	}

	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashWorker"));
}

void HashWorker::start() {
	boost::thread_group tg;

	LOG4CPLUS_INFO(logger, "Starting " << numOfWorkers << " worker thread(s)");

	for(int i = 0; i < numOfWorkers; i++) {
		boost::thread *t = new boost::thread(&HashWorker::doWork, this);
		tg.add_thread(t);
		LOG4CPLUS_INFO(logger, "Worker thread " << i << " started");
	}

	tg.join_all();

	LOG4CPLUS_INFO(logger, "All worker thread(s) have terminated");
}

void HashWorker::clear() {
	boost::mutex::scoped_lock(workQueueMutex);
	imagePaths.clear();
}

path HashWorker::getWork() {
	boost::mutex::scoped_lock(workQueueMutex);

	if (!imagePaths.empty()) {
		path next = imagePaths.back();
		imagePaths.pop_back();
		return next;
	} else {
		return NULL;
	}
}

void HashWorker::doWork() {
	ImagePHash iph(32, 9);
	int64_t pHash = 0;
	std::string filepath;
	Database::db_data data;

	while (!imagePaths.empty()) {
		path image = getWork();

		if (image.empty() || !boost::filesystem::exists(image)) {break;}

		try {
			filepath = image.string();
			data = Database::db_data(image);
			pHash = iph.getLongHash(filepath);
			data.pHash = pHash;
			data.status = Database::OK;
			LOG4CPLUS_DEBUG(logger, pHash << " - " << image);
			db.add(data);
		} catch (Magick::Exception &e) {
			LOG4CPLUS_WARN(logger, "Failed to process image " << filepath << " : " << e.what());
		}
	}
}
