/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file FilterRecord.hpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 */

#ifndef FILTERRECORD_HPP_
#define FILTERRECORD_HPP_

#include <inttypes.h>
#include <string>

#include <odb/core.hxx>

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("filterrecord")
class FilterRecord {
public:
	FilterRecord(uint64_t pHash, std::string reason);

	bool is_valid();

	#pragma db id auto
	int filter_id;

#pragma db index member(pHash) unique
	uint64_t pHash;

#pragma db value_not_null
	std::string reason;

private:
	FilterRecord();
	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* FILTERRECORD_HPP_ */
