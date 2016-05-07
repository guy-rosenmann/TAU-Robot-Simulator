#ifndef __STRING_UTILS__H_
#define __STRING_UTILS__H_

#include <cstring>
#include <sstream>

class StringUtils
{
public:
	static bool endsWith(const std::string& value, const std::string& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	template <typename T>
	static std::string numberToString(const T num)
	{
		std::stringstream ss;
		ss << num;
		return ss.str();
	}


	static std::string getWithTrailingSlash(const char* path)
	{
		if (path == NULL) return "";
		std::string sPath(path);
		if (!sPath.empty() && *sPath.rbegin() != '/' && *sPath.rbegin() != '\\')
		{
			return (sPath + "/");
		}
		return path;
	}
};


#endif // __STRING_UTILS__H_