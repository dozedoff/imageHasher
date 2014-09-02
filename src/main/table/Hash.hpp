/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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
	Hash();

	std::string get_sha256();
	uint64_t get_pHash();
	bool is_valid();

private:
#pragma db id auto
	int hash_id;

#pragma db index member(sha256) unique
#pragma db index member(pHash)
	uint64_t pHash;
	std::string sha256;

	friend class odb::access;
};

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */

#endif /* HASH_HPP_ */
