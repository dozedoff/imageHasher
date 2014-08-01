/*
 * <one line to give the program's name and a brief idea of what it does.>
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
 * @file Hash.hpp
 *
 * @date 1 Aug 2014
 * @author Nicholas Wright
 */
#ifndef HASH_HPP_
#define HASH_HPP_

#include <inttypes.h>
#include <string>

#include <odb/core.hxx>

namespace imageHasher {
namespace db {
namespace table {

#pragma db object
class Hash {
public:
	Hash(std::string sha256, uint64_t pHash);
	std::string get_sha256();
	uint64_t get_pHash();

private:
#pragma db id auto
	int hash_id;

	uint64_t pHash;
	std::string sha256;

	friend class odb::access;
	Hash();
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* HASH_HPP_ */
