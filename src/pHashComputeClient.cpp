/* The MIT License (MIT)
 * Copyright (c) 2014 Nicholas Wright
 * http://opensource.org/licenses/MIT
 */

/**
 * @file pHashComputeClient.cpp
 *
 * @date 9 Apr 2015
 * @author Nicholas Wright
 * @brief an undocumented file
 * 
 * Some more details about this file.
 */

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <iostream>
#include <iomanip>

#include "commoncpp.hpp"
#include "main/pHashCompute.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace boost::log::trivial;
using std::string;

class pHashComputeClient {
public:
	pHashComputeClient();
	virtual ~pHashComputeClient();
	int run(int, char* []);

private:
	boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
	imageHasher::pHashCompute *phc;

	string remoteIp;
	int pushPort;
	int pullPort;
	int threads;
};

int main(int argc, char* argv[]) {
	pHashComputeClient* pcc = new pHashComputeClient();
	int exitValue = pcc->run(argc, argv);
	delete pcc;
	return exitValue;
}

int pHashComputeClient::run(int argc, char* argv[]) {
	po::options_description desc = po::options_description("Allowed options");
	po::options_description allOptions;

	desc.add_options()
			("help", "Display this help message")
			("remote", po::value<string>(), "IP of remote server who issues tasks")
			("push-port", po::value<int>(),"Remote port for pushing tasks")
			("pull-port", po::value<int>(), "Remote port for pulling results")
			("threads", po::value<int>(), "Set the number of worker threads, overrides the 'auto' option")
			("auto", "Try to auto-detect the number of cores and create as many worker threads");

	allOptions.add(desc);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(allOptions).run(), vm);
	po::notify(vm);

	if (vm.count("help") > 0) {
		std::cout << desc << "\n";
		exit(0);
	}

	if (vm.count("remote")) {
		this->remoteIp = vm["remote"].as<string>();
	}

	if (vm.count("push-port")) {
		this->pushPort = vm["push-port"].as<int>();
	}

	if (vm.count("pull-port")) {
		this->pullPort = vm["pull-port"].as<int>();
	}

	if (vm.count("threads")) {
		this->threads = vm["threads"].as<int>();
	} else if (vm.count("auto")) {
		this->threads = boost::thread::hardware_concurrency();
	}

	this->phc = new imageHasher::pHashCompute(this->remoteIp,this->pushPort, this->pullPort, this->threads);

	return 0;
}

pHashComputeClient::pHashComputeClient() {
	phc = NULL;

	this->remoteIp = "127.0.0.1";
	this->pullPort = 4600;
	this->pushPort = 4700;
	this->threads = 1;
}

pHashComputeClient::~pHashComputeClient() {
	delete(phc);
}


