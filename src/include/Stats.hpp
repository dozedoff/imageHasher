/* The MIT License (MIT)
 * Copyright (c) <2013> <Nicholas Wright>
 * http://opensource.org/licenses/MIT
 */

/*
 * Stats.hpp
 *
 *  Created on: 15 Nov 2013
 *      Author: Nicholas Wright
 */

#ifndef STATS_HPP_
#define STATS_HPP_

#include <string>
#include <list>
#include <vector>
#include <map>

typedef std::pair<std::string, int> wordFreq;

class Stats {
public:
	void addValue(std::string value);
	void addValue(std::list<std::string> values);

	void clear();

	std::vector<wordFreq> getStats();
	virtual ~Stats();
private:
	std::map<std::string, int> frequency;

	bool sortByCount(wordFreq first, wordFreq second);
	void addToFrequency(std::string);
};

#endif /* STATS_HPP_ */
