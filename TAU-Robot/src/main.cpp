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
	Simulator simulator(config);

	simulator.simulate();
	
	getchar();
	return 0;
}