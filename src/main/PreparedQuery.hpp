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
 * @file PreparedQuery.hpp
 *
 * @date 6 Aug 2014
 * @author Nicholas Wright
 */

#ifndef PREPAREDQUERY_HPP_
#define PREPAREDQUERY_HPP_

#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/connection.hxx>

#include "table/ImageRecord.hpp"

#include <memory>

namespace imageHasher {
namespace db {

class PreparedQuery {
public:
	PreparedQuery(odb::database *db);
	virtual ~PreparedQuery();

	odb::prepared_query<imageHasher::db::table::ImageRecord> get_imagerecord_path_query(std::string *& path);
	odb::prepared_query<imageHasher::db::table::ImageRecord> get_files_with_path_query(std::string *& path);
	odb::prepared_query<imageHasher::db::table::Hash> get_hash_with_sha_query(std::string *& path);

private:
	odb::database *db;
	static void	imageRecord_query_factory (const char* query_name, odb::connection& connection);
	static void	path_query_factory (const char* query_name, odb::connection& connection);
	static void	hash_sha_query_factory (const char* query_name, odb::connection& connection);
};

} /* namespace db */
} /* namespace imageHasher */

#endif /* PREPAREDQUERY_HPP_ */
