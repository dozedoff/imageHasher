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
 * @file BadFileRecord.hpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 */

#ifndef BADFILERECORD_HPP_
#define BADFILERECORD_HPP_

#include <odb/core.hxx>

#include <string>

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("badfilerecord")
class BadFileRecord {
public:
	BadFileRecord();

private:
	#pragma db id
	std::string path;

	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* BADFILERECORD_HPP_ */
