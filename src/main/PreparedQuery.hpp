/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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
#include <inttypes.h>

namespace imageHasher {
namespace db {

class PreparedQuery {
public:
	PreparedQuery(odb::database *db);
	virtual ~PreparedQuery();

	odb::prepared_query<imageHasher::db::table::ImageRecord> get_imagerecord_path_query(std::string *& path);
	odb::prepared_query<imageHasher::db::table::ImageRecord> get_files_with_path_query(std::string *& path);
	odb::prepared_query<imageHasher::db::table::Hash> get_hash_query(std::string *& sha);
	odb::prepared_query<imageHasher::db::table::Hash> get_hash_query(uint64_t *& phash);

private:
	odb::database *db;
	static void	imageRecord_query_factory (const char* query_name, odb::connection& connection);
	static void	path_query_factory (const char* query_name, odb::connection& connection);
	static void	hash_sha_query_factory (const char* query_name, odb::connection& connection);
	static void	hash_phash_query_factory (const char* query_name, odb::connection& connection);
};

} /* namespace db */
} /* namespace imageHasher */

#endif /* PREPAREDQUERY_HPP_ */
