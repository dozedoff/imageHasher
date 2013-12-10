/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * Stats.cpp
 *
 *  Created on: 15 Nov 2013
 *      Author: Nicholas Wright
 */

#include "../include/Stats.hpp"

void Stats::addValue(std::string value) {
}

void Stats::addValue(std::list<std::string> values) {

}

void Stats::clear() {
	frequency.clear();
}

std::vector<wordFreq> Stats::getStats() {
	std::vector<wordFreq> stats;

	return stats;
}

Stats::~Stats() {
	// TODO Auto-generated destructor stub
}

bool Stats::sortByCount(wordFreq first, wordFreq second) {
	return false;
}

void Stats::addToFrequency(std::string allocator) {
}
