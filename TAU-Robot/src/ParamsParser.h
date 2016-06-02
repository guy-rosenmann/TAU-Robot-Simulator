#ifndef __PARAMS_PARSER__H_
#define __PARAMS_PARSER__H_

#include <map>
#include <string>

using namespace std;

class ParamsParser
{
	static const char* const _options[];
	static const char* const _flags[];
	static bool _wasUsageMessagePrinted;
	static const char* _usageMessage;

	map<string, string> _params;
	
public:

	ParamsParser() = delete;
	ParamsParser(int argc, char* argv[]);

	bool has_key(const string& key) const;

	const char* operator[](const string& key) const;
	const char* operator[](const char* key) const;

	static void printUsage();
};


#endif //__PARAMS_PARSER__H_