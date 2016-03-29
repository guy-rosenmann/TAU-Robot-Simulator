#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "ParamsParser.h"
#include "Simulator.h"

int main(int argc, char* argv[])
{
	std::srand((unsigned int)std::time(0)); // initialize random seed
	
	ParamsParser params(argc, argv);
	
	const char* conf_path = NULL, * house_path = NULL;
	if (params.has_key("-config"))
	{
		conf_path = params["-config"];
	}
	if (params.has_key("-house_path"))
	{
		house_path = params["-house_path"];
	}

	Configuration config(conf_path);
	if (config.isReady())
	{
		Simulator simulator(config, house_path);
		simulator.simulate();
	}
	else
	{
		cout << "[ERROR] Failed to load configuration file. Terminating..." << endl;
	}

#if defined(_DEBUG_) || defined(_RELEASE_)
	getchar();
#endif
	
	return 0;
}