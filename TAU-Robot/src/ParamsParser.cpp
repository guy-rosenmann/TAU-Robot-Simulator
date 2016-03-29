#include "ParamsParser.h"

#include <iostream>
#include <cstring>

using namespace std;


const char* const ParamsParser::_options[] = {
	"-config",
	"-house_path"
};


ParamsParser::ParamsParser(int argc, char* argv[])
{
	bool printUsage = false;

	for (int i = 1; i < argc; ++i)
	{
		unsigned int j, optionsSize = sizeof(_options) / sizeof(*_options);
		for (j = 0; j < optionsSize; ++j)
		{
			if (!strcmp(argv[i], _options[j]) && (i+1 < argc))
			{
				_params[argv[i]] = argv[i + 1];
				++i;
				break;
			}
		}
		if (j == optionsSize)
		{
			cout << "[WARN] Incompatible argument: " << argv[i] << endl;
			printUsage = true;
		}
	}

	if (printUsage)
	{
		cout << "USAGE: simulator [-config <path>] [-house_path <path>]" << endl;
		cout << "       - config:\tconfiguration file location path" << endl;
		cout << "       - house_path:\thouse files directory path" << endl;
		cout << "		Default paths: current working directory" << endl;
		cout << endl;
	}
}


bool ParamsParser::has_key(const string& key)
{
	map<string, string>::iterator it = _params.find(key);
	return (it != _params.end());
}