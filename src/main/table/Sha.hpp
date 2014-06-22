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
 * @file Sha.hpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */
#ifndef SHA_HPP_
#define SHA_HPP_

#include <string>
#include <odb/core.hxx>

namespace imageHasher {
namespace db {
namespace table {

#pragma db object table("sha_hash")
class Sha {
public:
	Sha();

private:
	#pragma db id auto
	int id;

	std::string sha256;

	friend class odb::access;

};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* SHA_HPP_ */
