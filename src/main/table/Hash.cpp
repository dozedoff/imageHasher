/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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

bool Hash::is_valid() {
	if(this->sha256.empty()) {
		return false;
	}else{
		return true;
	}
}

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */
