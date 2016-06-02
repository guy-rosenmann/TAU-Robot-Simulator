#include "ParamsParser.h"

#include <iostream>
#include <cstring>

using namespace std;


const char* const ParamsParser::_options[] = {
	"-config",
	"-house_path",
	"-algorithm_path",
	"-score_formula",
	"-threads"
};


const char* const ParamsParser::_flags[] = {
	"-video"
};


bool ParamsParser::_wasUsageMessagePrinted = false;
const char* ParamsParser::_usageMessage = "Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>] [-score_formula <score .so path>] [-threads <num threads>] [-video]";


ParamsParser::ParamsParser(int argc, char* argv[])
{
	bool shouldPrintUsage = false;
	unsigned int optionsSize = sizeof(_options) / sizeof(*_options);
	unsigned int flagsSize = sizeof(_flags) / sizeof(*_flags);
	
	for (int i = 1; i < argc; ++i)
	{
		unsigned int j, k;
		for (j = 0; j < optionsSize; ++j)
		{
			if (!strcmp(argv[i], _options[j]) && (i+1 < argc))
			{
				_params[argv[i]] = argv[i + 1];
				++i;
				break;
			}
		}
		for (k = 0; k < flagsSize; ++k)
		{
			if (!strcmp(argv[i], _flags[k]))
			{
				_params[argv[i]] = "true";
				break;
			}
		}
		if (j == optionsSize && k == flagsSize)
		{
#ifdef _DEBUG_
			cout << "[WARN] Incompatible argument: " << argv[i] << endl;
#endif
			shouldPrintUsage = true;
		}
	}

	if (shouldPrintUsage)
	{
		ParamsParser::printUsage();
	}
}


bool ParamsParser::has_key(const string& key) const
{
	map<string, string>::const_iterator it = _params.find(key);
	return (it != _params.end());
}


const char* ParamsParser::operator[](const string& key) const
{
	if (this->has_key(key))
	{
		return _params.at(key).c_str();
	}
	return NULL;
}


const char* ParamsParser::operator[](const char* key) const
{
	string strKey = key;
	return this->operator[](strKey);
}


void ParamsParser::printUsage()
{
	if (!ParamsParser::_wasUsageMessagePrinted)
	{
		cout << ParamsParser::_usageMessage << endl;
		ParamsParser::_wasUsageMessagePrinted = true;
	}
}