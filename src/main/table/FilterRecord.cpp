/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file FilterRecord.cpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 * @brief Storage class for filter records
 */
#include "FilterRecord.hpp"

namespace imageHasher {
namespace db {
namespace table {

bool FilterRecord::is_valid() {
	return !this->reason.empty();
}

FilterRecord::FilterRecord() {
	this->filter_id = 0;
	this->pHash = 0;
	this->reason = "";
}

FilterRecord::FilterRecord(uint64_t pHash, std::string reason) {
	this->filter_id = 0;
	this->pHash = pHash;
	this->reason = reason;
}

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */
