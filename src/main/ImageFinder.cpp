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
 * ImageFinder.cpp
 *
 *  Created on: 13 Aug 2013
 *      Author: nicholas
 */

#include "../include/ImageFinder.hpp"

#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>

using namespace boost::filesystem;

std::vector<path> imageExtensions;

ImageFinder::ImageFinder() {
	createExtensionList();
}

std::list<boost::filesystem::path> ImageFinder::getImages(boost::filesystem::path dirPath) {
	std::list<boost::filesystem::path> imagePaths;

	for ( boost::filesystem::recursive_directory_iterator endIter, dirIter(dirPath); dirIter != endIter; ++dirIter ) {
		if(is_regular_file(dirIter->status())){
			if(isImageExtension(dirIter->path().extension())){
				path currentPath = current_path();
				path absolutePath = canonical(*dirIter, currentPath);
				imagePaths.push_back(absolutePath);
			}
		}
	}

	return imagePaths;
}

bool ImageFinder::isImageExtension(path extension){
	for(std::vector<path>::iterator itr = imageExtensions.begin(); itr != imageExtensions.end(); ++itr){
		if(extension == *itr) {
			return true;
		}
	}

	return false;
}

void ImageFinder::createExtensionList() {
	imageExtensions.push_back(path(".jpg"));
	imageExtensions.push_back(path(".png"));
	imageExtensions.push_back(path(".gif"));
}
