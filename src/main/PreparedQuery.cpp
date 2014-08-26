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
 * @file PreparedQuery.cpp
 *
 * @date 6 Aug 2014
 * @author Nicholas Wright
 * @brief This class returns prepared queries and initialises them if needed.
 */
#include "PreparedQuery.hpp"

#include <odb/sqlite/database.hxx>
#include <odb/transaction.hxx>
#include <odb/result.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/exceptions.hxx>
#include <odb/prepared-query.hxx>

#include "table/ImageRecord.hpp"
#include "table/ImageRecord-odb.hxx"

#include "table/FilterRecord.hpp"
#include "table/FilterRecord-odb.hxx"

#include "table/Hash.hpp"
#include "table/Hash-odb.hxx"

#include <memory>

#define ih_query "image-hash-path-query"
#define path_query "get-files-with-path-query"
#define hash_sha_query "hash-sha-query"
#define hash_phash_query "hash-phash-query"

using namespace imageHasher::db::table;

namespace imageHasher {
namespace db {

PreparedQuery::PreparedQuery(odb::database *db) {
	this->db = db;

	db->query_factory(ih_query, &imageRecord_query_factory);
	db->query_factory(path_query, &path_query_factory);
	db->query_factory(hash_sha_query, &hash_sha_query_factory);
	db->query_factory(hash_phash_query, &hash_phash_query_factory);
}

PreparedQuery::~PreparedQuery() {
	// TODO Auto-generated destructor stub
}

odb::prepared_query<ImageRecord> PreparedQuery::get_imagerecord_path_query(std::string*& path) {
	odb::prepared_query<ImageRecord> pq (db->lookup_query<ImageRecord>(ih_query,path));
	return pq;
}

odb::prepared_query<imageHasher::db::table::ImageRecord> PreparedQuery::get_files_with_path_query(std::string*& path) {
	odb::prepared_query<ImageRecord> pq (db->lookup_query<ImageRecord>(path_query,path));
	return pq;
}

odb::prepared_query<imageHasher::db::table::Hash> PreparedQuery::get_hash_query(std::string*& sha) {
	odb::prepared_query<Hash> pq(db->lookup_query<Hash>(hash_sha_query, sha));
	return pq;
}

odb::prepared_query<imageHasher::db::table::Hash> PreparedQuery::get_hash_query(uint64_t*& phash) {
	odb::prepared_query<Hash> pq(db->lookup_query<Hash>(hash_phash_query, phash));
	return pq;
}

void PreparedQuery::imageRecord_query_factory(const char* query_name, odb::connection& connection) {
	typedef odb::query<ImageRecord> query;

	std::auto_ptr<std::string> p (new std::string);
	query q (query::path == query::_ref (*p));
	odb::prepared_query<ImageRecord> pq (connection.prepare_query<ImageRecord> (query_name, q));
	connection.cache_query (pq, p);
}

void PreparedQuery::path_query_factory(const char* query_name, odb::connection& connection) {
	typedef odb::query<ImageRecord> query;

	std::auto_ptr<std::string> p(new std::string);
	query q(query::path.like(query::_ref(*p)));
	odb::prepared_query<ImageRecord> pq(connection.prepare_query<ImageRecord>(query_name, q));
	connection.cache_query(pq, p);
}

void PreparedQuery::hash_sha_query_factory(const char* query_name, odb::connection& connection) {
	typedef odb::query<Hash> query;

	std::auto_ptr<std::string> p(new std::string);
	query q(query::sha256 == query::_ref(*p));
	odb::prepared_query<Hash> pq(connection.prepare_query<Hash>(query_name, q));
	connection.cache_query(pq, p);
}

void PreparedQuery::hash_phash_query_factory(const char* query_name, odb::connection& connection) {
	typedef odb::query<Hash> query;

	std::auto_ptr<uint64_t> p(new uint64_t);
	query q(query::pHash == query::_ref(*p));
	odb::prepared_query<Hash> pq(connection.prepare_query<Hash>(query_name, q));
	connection.cache_query(pq, p);
}

} /* namespace db */
} /* namespace imageHasher */
