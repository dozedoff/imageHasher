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


namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;
using namespace log4cplus;

ImageFinder imageFinder;
Database db;
ImagePHash iph;

Logger logger;


class HashUtil {
public:
	int run(int, char**);
	HashUtil();
private:
	bool isValidPath(string);
	void filter(path, string);
	void prune(fs::path);
};

int main(int argc, char* argv[]) {
	HashUtil hu;
	return hu.run(argc, argv);
}

HashUtil::HashUtil(){
	PropertyConfigurator config("logs.properties");
	config.configure();
	logger = Logger::getInstance(LOG4CPLUS_TEXT("HashUtil"));
}

int HashUtil::run(int argc, char* argv[]) {
	po::options_description desc = po::options_description("Allowed options");
	po::options_description hidden("Hidden options");
	po::options_description allOptions;

	po::positional_options_description pos;
	pos.add("path", -1);

	desc.add_options()
			("help", "Display this help message")
			("filter", po::value<string>(), "Add files in the directory and subdirectories into filter list")
			("prune", "Remove non-existing file paths from the database")
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
	}else if((vm.count("filter") == 0) && (vm.count("prune") == 0)){
	cout << "No operation selected, aborting.\n";
		exit(2);
	}

	if(vm.count("filter")){
		cout << "Adding files to filter with reason \"" << vm["filter"].as<string>() << "\"\n";
	}

	if(vm.count("prune")) {
		cout << "Pruning database.\n";
	}

	cout << "Folders to process:\n";
	vector<string> paths = vm["path"].as<vector<string> >();

	for (vector<string>::iterator ite = paths.begin(); ite != paths.end(); ++ite) {
		if (isValidPath(*ite)) {
			cout << *ite << " - OK" << "\n";
		} else {
			cout << *ite << " - INVALID" << "\n";
			paths.erase(ite);
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
	}

	return 0;
}

bool HashUtil::isValidPath(string path) {
	fs::path p(path);
	return fs::exists(p) && fs::is_directory(p);
}

void HashUtil::filter(path directory, string reason){
	list<fs::path> images = imageFinder.getImages(directory);
	LOG4CPLUS_INFO(logger, "Filtering " << images.size() << " image(s) for " << directory);

	for(list<fs::path>::iterator ite = images.begin(); ite != images.end(); ++ite) {
		int64_t pHash = iph.getLongHash(ite->string());

		Database::db_data data;
		data.pHash = pHash;
		data.reason = reason;
		data.status = Database::FILTER;

		db.add(data);
	}

	db.flush();
}

void HashUtil::prune(fs::path directory) {
	list<path> files = db.getFilesWithPath(directory);
	int pruneCount = 0;

	LOG4CPLUS_INFO(logger, "Found " << files.size() << " entries for " << directory);

	for(list<path>::iterator ite = files.begin(); ite != files.end(); ++ite) {
		if(! fs::exists(*ite)) {
			db.prunePath(*ite);
			pruneCount++;
			LOG4CPLUS_DEBUG(logger, "Pruning path " << *ite << " from the database");
		}
	}

	LOG4CPLUS_INFO(logger, "Pruned " << pruneCount << " of " << files.size() << " entries");
}
