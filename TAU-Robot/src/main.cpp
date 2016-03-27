#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "ParamsParser.h"
#include "Simulator.h"

int main(int argc, char* argv[])
{
	std::srand((unsigned int)std::time(0)); // initialize random seed
	
	ParamsParser params(argc, argv);
	
	string conf_path = "";
	if (params.has_key("-config"))
	{
		conf_path = params["-config"];
	}

	Configuration config(conf_path);
	if (config.isReady())
	{
		Simulator simulator(config);
		simulator.simulate();
	}
	else
	{
		cout << "[ERROR] Failed to load configuration from file. Terminating..." << endl;
	}

	return 0;
}