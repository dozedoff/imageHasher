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
 * @file Settings.hpp
 *
 * @date 27 Jul 2014
 * @author Nicholas Wright
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <odb/core.hxx>
#include <map>
#include <string>

namespace db {
namespace table {

class Settings {
public:
	Settings(std::string id){this->id = id;}

	void set_value(std::string key, std::string value);
	std::string get_value(std::string key);

private:
	#pragma db id
	std::string id;

	std::map<std::string,std::string> settings;

	friend class odb::access;
	Settings();
};

} /* namespace table */
} /* namespace db */
#endif /* SETTINGS_HPP_ */
