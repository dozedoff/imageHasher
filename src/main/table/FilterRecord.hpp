/*
 * imageHasher - C++ version of the Hashing module from SimilarImage
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
	uint64_t pHash;

	std::string reason;

private:
	FilterRecord();
	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* FILTERRECORD_HPP_ */
