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
 * @file Phash.hpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 */
#ifndef PHASH_HPP_
#define PHASH_HPP_

#include <inttypes.h>

#include <odb/core.hxx>

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("phash_hash")
class Phash {
public:
	Phash();

private:
	#pragma db id auto
	int id;

	uint64_t pHash;

	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* PHASH_HPP_ */
