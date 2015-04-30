/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file HashJob.hpp
 *
 * @date 30 Apr 2015
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#ifndef SRC_MAIN_TABLE_HASHJOB_HPP_
#define SRC_MAIN_TABLE_HASHJOB_HPP_

#include <odb/core.hxx>

#include <string>

#include "ImageRecord.hpp"

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("hashjob")
class HashJob {
public:
	HashJob();
	virtual ~HashJob();

private:
	#pragma db id auto
	unsigned int job_id;

	#pragma db index member(sha256)
	std::string sha256;

	#pragma db index member(imageRecord) unique
	#pragma db value_not_null
	ImageRecord *imageRecord;

	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* SRC_MAIN_TABLE_HASHJOB_HPP_ */
