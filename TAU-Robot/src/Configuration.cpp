#include "Configuration.h"
#include "StringUtils.h"
#include "ParamsParser.h"

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
	string path = StringUtils::getWithTrailingSlash(iniPath_ != NULL ? iniPath_ : ".");
	_successful = loadFromPath(path);
}



bool Configuration::loadFromPath(const string& iniPath_)
{
	string fullPath = iniPath_ + Configuration::configFileName; // add config file name to path
	struct stat buf;
	if (stat(fullPath.c_str(), &buf) != 0)
	{
		// ini file is missing
		ParamsParser::printUsage();
		cout << "cannot find " << Configuration::configFileName << " file in '" << StringUtils::getFullPath(iniPath_) << "'" << endl;
		return false;
	}

	ifstream fin(fullPath.c_str());
	if (!fin.good())
	{
		cout << Configuration::configFileName << " exists in '" << StringUtils::getFullPath(iniPath_) << "' but cannot be opened" << endl;
		return false;
	}

	// the file exists and readable
	string line;
	while (getline(fin, line))
	{
		this->processLine(line);
	}
	fin.close();

	if (_badParams.size() > 0)
	{
		cout << Configuration::configFileName << " having bad values for " << _badParams.size() << " parameter(s): ";
		for (vector<string>::iterator it = _badParams.begin(); it != _badParams.end(); ++it)
		{
			cout << *it;
			if (it + 1 != _badParams.end()) cout << ", ";
		}
		cout << endl;
	}

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
	bool hadBadParam = false;
	unsigned int missingCount = 0, mandatoryParamsSize = sizeof(Configuration::mandatoryParams) / sizeof(*Configuration::mandatoryParams);
	for (unsigned int i = 0; i < mandatoryParamsSize; ++i)
	{
		map<string, int>::const_iterator it = _params.find(Configuration::mandatoryParams[i]);
		if (it == _params.end())
		{
			if (std::find(_badParams.begin(), _badParams.end(), Configuration::mandatoryParams[i]) == _badParams.end()) // add only if does not exist with bad value
			{
				missingCount++;
				missingParams += string(Configuration::mandatoryParams[i]) + string(", ");
			}
			else
			{
				hadBadParam = true;
			}
		}
	}

	if (missingCount != 0)
	{
		cout << Configuration::configFileName << " missing " << missingCount << " parameter(s): " << missingParams.substr(0, missingParams.size() - 2) << endl;
		return false;
	}
	else if (hadBadParam)
	{
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


bool Configuration::processLine(const string& line)
{
	if (line.size() == 0) return true;
	vector<string> tokens = Configuration::split(line, '=');
	if (tokens.size() != 2)
	{
		goto bad_param;
	}

	int num;
	try
	{
		num = stoi(tokens[1]);
	}
	catch (std::invalid_argument e)
	{
		goto bad_param;
	}
	catch (std::out_of_range e)
	{
		goto bad_param;
	}
	
	if (num >= 0)
	{
		_params[Configuration::trim(tokens[0])] = num;
		return true;
	}

bad_param:
	if (isMandatory(tokens[0].c_str()))
	{
		_badParams.push_back(tokens[0]);
	}
	return false;
}


bool Configuration::isMandatory(const char* param)
{
	unsigned int mandatoryParamsSize = sizeof(Configuration::mandatoryParams) / sizeof(*Configuration::mandatoryParams);
	for (unsigned int i = 0; i < mandatoryParamsSize; ++i)
	{
		if (string(Configuration::mandatoryParams[i]) == param)
		{
			return true;
		}
	}

	return false;
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