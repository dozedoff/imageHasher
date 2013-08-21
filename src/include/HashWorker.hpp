/*
 * HashWorker.h
 *
 *  Created on: 20 Aug 2013
 *      Author: nicholas
 */

#ifndef HASHWORKER_H_
#define HASHWORKER_H_

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "../include/Database.hpp"

using boost::filesystem::path;

class HashWorker {
public:
	HashWorker(std::list<path>* ,int);
	void start();

private:
	int numOfWorkers;
	std::list<path> imagePaths;
	boost::mutex workQueueMutex;
	log4cplus::Logger logger;
	Database db;

	typedef std::list<path> pathList;

	std::vector<pathList> splitWork(void);
	void doWork(void);
	path getWork();
	static void derp(int);
};

#endif /* HASHWORKER_H_ */
