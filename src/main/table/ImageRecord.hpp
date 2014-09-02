/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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
