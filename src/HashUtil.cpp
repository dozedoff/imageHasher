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
 * HashUil.cpp
 *
 *  Created on: 30 Aug 2013
 *      Author: nicholas
 */

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <iostream>

#include "include/ImageFinder.hpp"
#include "include/Database.hpp"
#include "../commoncpp/src/include/commoncpp.hpp"
#include "../commoncpp/src/include/hash/SHA.hpp"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;
using namespace log4cplus;

class HashUtil {
public:
	int run(int, char* []);
	HashUtil();
	~HashUtil();
private:
	ImageFinder* imageFinder;
	Database* db;
	ImagePHash* iph;
	SHA *sha;

	Logger logger;

	bool isValidPath(string);
	void filter(path, string);
	void prune(fs::path);
	void updateSha(fs::path);
};

int main(int argc, char* argv[]) {
	HashUtil* hu = new HashUtil();
	int exitValue = hu->run(argc, argv);
	delete hu;
	return exitValue;
}

HashUtil::~HashUtil() {
	delete iph;
	delete db;
	delete imageFinder;
	delete sha;
}

HashUtil::HashUtil(){
	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashUtil"));

	imageFinder = new ImageFinder();
	db = new Database();
	iph = new ImagePHash();
	sha = new SHA();
}

int HashUtil::run(int argc, char* argv[]) {
	LOG4CPLUS_INFO(logger, "HashUtil init...");
	po::options_description desc = po::options_description("Allowed options");
	po::options_description hidden("Hidden options");
	po::options_description allOptions;

	po::positional_options_description pos;
	pos.add("path", -1);

	desc.add_options()
			("help", "Display this help message")
			("filter", po::value<string>(), "Add files in the directory and subdirectories into filter list")
			("prune", "Remove non-existing file paths from the database")
			("sha", "Update SHA256 for all records")
	;

	hidden.add_options()
		("path", po::value<vector<string> >()->multitoken(), "Paths to process")
	;

	allOptions.add(desc).add(hidden);

	po::variables_map vm;
	po::store(po::command_line_parser(argc,argv).options(allOptions).positional(pos).run(), vm);
	po::notify(vm);

	if(vm.count("help") > 0) {
		cout << desc << "\n";
		exit(0);
	}else if(vm.count("path") == 0){
		cout << "No paths given, aborting.\n";
		exit(1);
	}else if((vm.count("filter") == 0) && (vm.count("prune") == 0) && (vm.count("sha") == 0)){
		cout << "No operation selected, aborting.\n";
		exit(2);
	}

	if(vm.count("filter")){
		cout << "Adding files to filter with reason \"" << vm["filter"].as<string>() << "\"\n";
	}

	if(vm.count("prune")) {
		cout << "Pruning database.\n";
	}

	if(vm.count("sha")){
		cout << "Updating SHA\n";
	}

	cout << "Folders to process:\n";
	vector<string> paths = vm["path"].as<vector<string> >();

	for (vector<string>::iterator ite = paths.begin(); ite != paths.end(); ++ite) {
		if (isValidPath(*ite)) {
			cout << *ite << " - OK" << "\n";
		} else {
			cout << *ite << " - INVALID" << "\n";
			ite = paths.erase(ite);
		}
	}


	//TODO add user confirmation here
	for (vector<string>::iterator ite = paths.begin(); ite != paths.end(); ++ite) {
		fs::path path(*ite);
		LOG4CPLUS_INFO(logger, "Processing directory " << path);

		if (vm.count("prune") > 0) {
			prune(path);
		}

		if(vm.count("filter") > 0) {
			filter(path, vm["filter"].as<string>());
		}

		if(vm.count("sha") > 0) {
			updateSha(path);
		}
	}

	return 0;
}

bool HashUtil::isValidPath(string path) {
	fs::path p(path);
	return fs::exists(p) && fs::is_directory(p);
}

void HashUtil::filter(path directory, string reason){
	list<fs::path> images = imageFinder->getImages(directory);
	LOG4CPLUS_INFO(logger, "Filtering " << images.size() << " image(s) for " << directory);

	for(list<fs::path>::iterator ite = images.begin(); ite != images.end(); ++ite) {
		int64_t pHash = iph->getLongHash(ite->string());

		Database::db_data data;
		data.pHash = pHash;
		data.reason = reason;
		data.status = Database::FILTER;

		db->add(data);
	}

	db->flush();
}

void HashUtil::prune(fs::path directory) {
	list<path> files = db->getFilesWithPath(directory);
	int pruneCount = 0;

	for(list<path>::iterator ite = files.begin(); ite != files.end(); ++ite) {
		LOG4CPLUS_DEBUG(logger, "Checking if " << *ite << " exists");

		if(fs::exists(*ite)) {
			ite = files.erase(ite);
		}else{
			pruneCount++;
		}
	}

	LOG4CPLUS_INFO(logger, "Found " << pruneCount << " files that no longe exist");
	db->prunePath(files);
	LOG4CPLUS_INFO(logger, "Pruned " << pruneCount << " of " << files.size() << " entries");
}

void HashUtil::updateSha(fs::path directory) {
	list<fs::path> images = imageFinder->getImages(directory);
	LOG4CPLUS_INFO(logger, "Updating SHA for " << images.size() << " image(s) for " << directory);

	for(list<fs::path>::iterator ite = images.begin(); ite != images.end(); ++ite) {
		if(!db->entryExists(*ite)) {
			LOG4CPLUS_DEBUG(logger, *ite << " not in database, skipping...");
			continue;
		}

		if (db->hasSHA(*ite)) {
			LOG4CPLUS_DEBUG(logger, *ite << " already has a SHA, skipping...");
			continue;
		}

		std::string sha256 = sha->sha256(*ite);

		Database::db_data data;
		data.status = Database::SHA;
		data.filePath = *ite;
		data.sha256 = sha256;

		db->add(data);
		LOG4CPLUS_DEBUG(logger, "Updating SHA for image " << *ite << " with " << sha256);
	}
}
