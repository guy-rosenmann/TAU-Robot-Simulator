#include "ParamsParser.h"


const char* const ParamsParser::_options[] = {
	"-config"
};


ParamsParser::ParamsParser(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		unsigned int j;
		for (j = 0; j < sizeof(_options); ++j)
		{
			if (!strcmp(argv[i], _options[j]) && (i+1 < argc))
			{
				_params[argv[i]] = argv[i + 1];
				++i;
				break;
			}
		}
		if (j == sizeof(_options))
		{
			printf("[ERROR] Illegal instruction\n");
		}
	}
}


bool ParamsParser::has_key(const string& key)
{
	map<string, string>::iterator it = _params.find(key);
	return (it != _params.end());
}