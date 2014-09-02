/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file ImageRecord.cpp
 *
 * @date 22 Jun 2014
 * @author Nicholas Wright
 * @brief Container for Imagerecords
 */
#include "ImageRecord.hpp"

namespace imageHasher {
namespace db {
namespace table {

ImageRecord::ImageRecord() {
	this->image_id = 0;
	this->path = "";
	this->hash = NULL;
}

ImageRecord::ImageRecord(std::string path, Hash *hash) {
	this->image_id = 0;
	this->path = path;
	this->hash = hash;
}

Hash ImageRecord::get_hash() {
	return *(this->hash);
}

bool ImageRecord::is_valid() {
	if(this->path.empty()) {
		return false;
	}else{
		return true;
	}
}

} /* namespace table */
} /* namespace db */
} /* namespace imageHasher */
