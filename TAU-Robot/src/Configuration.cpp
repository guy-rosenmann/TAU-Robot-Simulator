#include "Configuration.h"
#include "Constants.h"

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

string Configuration::configFileName = "config.ini";

const char* const Configuration::mandatoryParams[] = {
	"MaxStepsAfterWinner",
	"BatteryCapacity",
	"BatteryConsumptionRate",
	"BatteryRechargeRate"
};


Configuration::Configuration(const char* iniPath_)
{ 
	string fullPath;
	if (iniPath_ != NULL)
	{
		fullPath = iniPath_;
		// fix for missing back slash
		if (!fullPath.empty() && *fullPath.rbegin() != '/' && *fullPath.rbegin() != '\\')
		{
			fullPath += "/";
		}
		fullPath += Configuration::configFileName; // add config file name to path
	}
	else
	{
		fullPath = Configuration::configFileName;
	}

	_successful = loadFromFile(fullPath);
}



bool Configuration::loadFromFile(const string& iniPath_)
{
	struct stat buf;
	if (stat(iniPath_.c_str(), &buf) != 0)
	{
		// ini file is missing
		cout << USAGE_MSG << endl;
		return false;
	}

	ifstream fin(iniPath_.c_str());
	if (!fin.good())
	{
		cout << "config.ini exists in " << iniPath_ << " but cannot be opened" << endl;
		return false;
	}

	// the file exists and readable
	string line;
	while (getline(fin, line))
	{
		this->processLine(line);
	}
	fin.close();

	return checkAllParamsExistence();
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


bool Configuration::checkAllParamsExistence()
{
	string missingParams;
	unsigned int i, missingCount = 0, mandatoryParamsSize = sizeof(Configuration::mandatoryParams) / sizeof(*Configuration::mandatoryParams);
	for (i = 0; i < mandatoryParamsSize; ++i)
	{
		map<string, int>::const_iterator it = _params.find(Configuration::mandatoryParams[i]);
		if (it == _params.end())
		{
			missingCount++;
			missingParams += string(Configuration::mandatoryParams[i]) + string(", ");
		}
	}

	if (missingCount != 0)
	{
		cout << Configuration::configFileName << " missing " << missingCount << " parameter(s): " << missingParams.substr(0, missingParams.size() - 2) << endl;

		return false;
	}

	return true;
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

	int num;
	try
	{
		num = stoi(tokens[1]);
	}
	catch (std::invalid_argument e)
	{
		return;
	}
	catch (std::out_of_range e)
	{
		return;
	}
	_params[Configuration::trim(tokens[0])] = num;
}


// Not needed functions
/*

void Configuration::loadDefaultConfig()
{
	if (!this->loadFromFile(Configuration::configFileName))
	{
		this->writeConfigFile(Configuration::configFileName); // create the default config file
	}
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

*/