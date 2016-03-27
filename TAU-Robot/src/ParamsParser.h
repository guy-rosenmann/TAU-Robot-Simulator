#ifndef __PARAMS_PARSER__H_
#define __PARAMS_PARSER__H_

#include <map>
#include <string>

using namespace std;

class ParamsParser
{
	static const char* const _options[];
	
	map<string, string> _params;
	
public:

	ParamsParser() = delete;
	ParamsParser(int argc, char* argv[]);

	bool has_key(const string& key);

	const char* operator[](const string& key) const { return _params.at(key).c_str(); }
	const char* operator[](const char* key) const { return _params.at(key).c_str(); }
};


#endif //__PARAMS_PARSER__H_