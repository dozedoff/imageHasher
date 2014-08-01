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
 * @file Hash.cpp
 *
 * @date 1 Aug 2014
 * @author Nicholas Wright
 * @brief Storage class for hash values
 */
#include "Hash.hpp"

namespace imageHasher {
namespace db {
namespace table {

Hash::Hash(std::string sha256, uint64_t pHash) {
	this->hash_id = 0;
	this->sha256 = sha256;
	this->pHash = pHash;
}

std::string Hash::get_sha256() {
	return this->sha256;
}

uint64_t Hash::get_pHash() {
	return this->pHash;
}

Hash::Hash() {
	this->hash_id = 0;
	this->sha256 = "";
	this->pHash = 0;
}

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */
