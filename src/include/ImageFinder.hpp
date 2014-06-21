/*
 * ImageHasher - C++ version of the Hashing module from SimilarImage
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

/*
 * ImageFinder.h
 *
 *  Created on: 13 Aug 2013
 *      Author: nicholas
 */

#ifndef IMAGEFINDER_H_
#define IMAGEFINDER_H_

#include <boost/filesystem.hpp>
#include <list>

using namespace boost::filesystem;

class ImageFinder {
public:
	std::list<boost::filesystem::path> getImages(boost::filesystem::path);
	ImageFinder();
private:
	bool isImageExtension(path);
	void createExtensionList(void);
};

#endif /* IMAGEFINDER_H_ */
