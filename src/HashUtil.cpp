/*
 * HashUil.cpp
 *
 *  Created on: 30 Aug 2013
 *      Author: nicholas
 */

#include <boost/program_options.hpp>
#include <cstdio>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	po::options_description desc = po::options_description("Allowed options");
	desc.add_options()
			("help", "Display this help message")
			("filter", "Add files in the directory and subdirectories into filter list");
			("prune", "Remove non-existing file paths from the database")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	if(vm.count("help") > 0) {
		std::cout << desc << "\n";
	}
}

