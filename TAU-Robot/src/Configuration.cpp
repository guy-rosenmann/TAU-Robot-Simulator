#include "Configuration.h"

#include <iostream>
#include <fstream>
#include <sstream>

string Configuration::configFileName = "config.ini";


void Configuration::loadFromFile(const string& iniPath_)
{
	_params.clear();
	
	// defaults
	_params["MaxSteps"] = 1200;
	_params["MaxStepsAfterWinner"] = 200;
	_params["BatteryCapacity"] = 400;
	_params["BatteryConsumptionRate"] = 1;
	_params["BatteryRechargeRate"] = 20;

	ifstream fin(iniPath_.c_str());
	if (fin.good())
	{
		// the file exists and readable
		string line;
		while (getline(fin, line))
		{
			this->processLine(line);
		}
	}
	else
	{
		// use defaults (and create config file)
		this->writeConfigFile(Configuration::configFileName); // create the default config file
	}
	
	fin.close();
}

void Configuration::writeConfigFile(const string& iniPath_) const
{
	ofstream fout(iniPath_.c_str());

	for (map<string, int>::const_iterator itr = _params.begin(); itr != _params.end(); ++itr)
	{
		fout << itr->first << "=" << itr->second << endl;
	}

	fout.close();
}

string Configuration::toString() const
{
	string out;
	for (map<string, int>::const_iterator itr = _params.begin(); itr != _params.end(); ++itr)
	{
		if (itr != _params.begin())
		{
			out += "\n";
		}
		out += "parameters[" + itr->first + "] = " + to_string(itr->second);
	}
	return out;
}

std::vector<std::string> Configuration::split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::string Configuration::trim(std::string& str)
{
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);       //surfixing spaces
	return str;
}

void Configuration::processLine(const string& line)
{
	vector<string> tokens = Configuration::split(line, '=');
	if (tokens.size() != 2)
	{
		return;
	}
	_params[Configuration::trim(tokens[0])] = stoi(tokens[1]);
}