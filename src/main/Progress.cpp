/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * Progress.cpp
 *
 *  Created on: 26 Dec 2013
 *      Author: Nicholas Wright
 */

#include "../include/Progress.hpp"
#include <ncurses.h>

Progress::Progress() {
	initscr();
	init();
}

void Progress::update() {
	mvwprintw(stdscr,0,0,"Processed %d of %d files", getAllProcessed(), totalFiles);
}

void Progress::setTotal(int total) {
	totalFiles = total;
}

void Progress::skip(int amount=1) {
	skippedFiles += amount;
}

void Progress::processed(int amount=1) {
	processedFiles += amount;
}

void Progress::bad(int amount=1) {
	badFiles += amount;
}

void Progress::init() {
	currentDirectory = "NONE";
	totalFiles = 0;
	processedFiles = 0;
	skippedFiles = 0;
	badFiles = 0;
}

int Progress::getAllProcessed() {
	return processedFiles + skippedFiles + badFiles;
}

void Progress::reset() {
	init();
}

Progress::~Progress() {
	endwin();
}

