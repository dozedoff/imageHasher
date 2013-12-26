/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * Progress.hpp
 *
 *  Created on: 26 Dec 2013
 *      Author: Nicholas Wright
 */

#ifndef PROGRESS_HPP_
#define PROGRESS_HPP_

#include <string>

class Progress {
public:
	Progress();
	virtual ~Progress();

	void update();
	void reset();

	void setTotal(int total);
	void skip(int amount);
	void processed(int amount);
	void bad(int amount);

	int getAllProcessed();

private:
	std::string currentDirectory;
	int totalFiles, processedFiles, skippedFiles, badFiles;

	void init();
};

#endif /* PROGRESS_HPP_ */
