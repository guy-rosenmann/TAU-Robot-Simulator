#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "ParamsParser.h"
#include "Simulator.h"

int main(int argc, char* argv[])
{
	std::srand((unsigned int)std::time(0)); // initialize random seed for algorithm
	
	ParamsParser params(argc, argv);
	const char* conf_path, *house_path, *algorithm_path;
	conf_path = params["-config"];
	house_path = params["-house_path"];
	algorithm_path = params["-algorithm_path"];

	Configuration config(conf_path);
	if (!config.isReady())
	{
		cout << "[ERROR] Failed to load configuration file. Terminating..." << endl;
		return -1;

	}

	Simulator simulator(config, house_path, algorithm_path);
	if (!simulator.isReady())
	{
		return -1;
	}

	simulator.simulate();

#if defined(_DEBUG_) || defined(_RELEASE_)
	getchar();
#endif
	
	return 0;
}