/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
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
