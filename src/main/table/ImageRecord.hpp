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
 * @file ImageRecord.hpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 */
#ifndef IMAGERECORD_HPP_
#define IMAGERECORD_HPP_

#include <odb/core.hxx>

#include <string>

#include "Hash.hpp"

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("imagerecord")
class ImageRecord {
public:
	ImageRecord(std::string path, Hash *hash);
	ImageRecord();

	Hash get_hash();
	bool is_valid();

	const std::string& getPath() const {
		return path;
	}

private:
	#pragma db id auto
	int image_id;

#pragma db index member(path) unique
	std::string path;
#pragma db index member(hash)
#pragma db value_not_null
	Hash *hash;

	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* IMAGERECORD_HPP_ */
