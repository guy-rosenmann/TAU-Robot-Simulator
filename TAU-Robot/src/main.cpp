#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "ParamsParser.h"
#include "Simulator.h"


int main(int argc, char* argv[])
{
	ParamsParser params(argc, argv);

	const char* conf_path, *house_path, *algorithm_path, *score_path, *threadsCount;
	conf_path = params["-config"];
	house_path = params["-house_path"];
	algorithm_path = params["-algorithm_path"];
	score_path = params["-score_formula"];
	threadsCount = params["-threads"];

	Configuration config(conf_path);
	if (!config.isReady()) goto error;

	{
		Simulator simulator(config, house_path, algorithm_path, score_path, threadsCount);
		if (!simulator.isReady()) goto error;
		simulator.simulate();
	}

#ifdef _WINDOWS_
	getchar();
#endif
	return 0;

error:
#ifdef _WINDOWS_
	getchar();
#endif
	return -1;
}
