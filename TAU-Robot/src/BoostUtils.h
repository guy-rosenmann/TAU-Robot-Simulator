#ifndef __BOOST_UTILS__H_
#define __BOOST_UTILS__H_


#include <boost/filesystem.hpp>
#include <iostream>

class BoostUtils
{
public:

	static std::string getFullPath(const char* path)
	{
		return (boost::filesystem::exists(path) ? boost::filesystem::canonical(path).string() : boost::filesystem::path(path).string());
	}

	static std::string getFullPath(const std::string path) { return BoostUtils::getFullPath(path.c_str()); }

	
	static bool createDirectoryIfNotExists(const string& dirPath_)
	{
		bool returnValue = true;
		boost::filesystem::path dirPath(dirPath_.c_str());
		if (!is_directory(dirPath.parent_path()))
		{
			returnValue = returnValue && createDirectoryIfNotExists(dirPath.parent_path().string());
		}
		return boost::filesystem::create_directory(dirPath);
	}
};


#endif // __BOOST_UTILS__H_