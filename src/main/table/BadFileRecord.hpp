/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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
