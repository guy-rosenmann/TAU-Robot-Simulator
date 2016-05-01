#ifndef __CONFIG_PARSER__H_
#define __CONFIG_PARSER__H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;


class Configuration
{
	static string configFileName;
	static const char* const mandatoryParams[];

	map<string, int>	_params;
	bool				_successful = false; // did last loading of config was OK

public:

	Configuration() :_successful(false){}
	Configuration(const char* iniPath_);
	Configuration(const Configuration& other) : _params(other._params), _successful(other._successful) {}

	bool isReady() { return _successful; }

	bool loadFromFile(const string& iniPath_);

	map<string, int> getParams() const { return _params; }

	int& operator[](const string& key) { return _params[key]; }
	int& operator[](const char* key) { return _params[key]; }
	const int operator[](const string& key) const { return _params.at(key); }
	const int operator[](const char* key) const { return _params.at(key); }
	Configuration& operator=(const Configuration& other) { _params = other._params; _successful = other._successful; return *this; }

	string toString() const;
	void print(ostream& out = cout) const { out << this->toString(); }
	friend ostream& operator<<(ostream& out, const Configuration& p) { p.print(out); return out; }

private:

	static std::vector<std::string> split(const std::string& s, char delim);
	static std::string trim(std::string& str);

	bool checkAllParamsExistence();
	void processLine(const string& line);
	
};


#endif //__CONFIG_PARSER__H_