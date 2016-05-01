#ifndef __SHARED_OBJECT_LOADER__H_
#define __SHARED_OBJECT_LOADER__H_

#include <string>
#include <boost/filesystem.hpp>
using namespace std;

#ifndef _WINDOWS_
#include <dlfcn.h>
#endif


class SharedObjectLoader
{
	void*					_handle = NULL;
	boost::filesystem::path _path;

public:

#ifndef _WINDOWS_
	SharedObjectLoader(const char* soPath_) : _path(soPath_) { _handle = dlopen(soPath_, RTLD_NOW); }
	~SharedObjectLoader() { if (_handle != NULL) dlclose(_handle); }

	void* getFunctionPointer(const char* funcName_) const
	{
		if (!isValid()) return NULL;
		return dlsym(_handle, funcName_);
	}
#else
	// for Windows tests only
	SharedObjectLoader::SharedObjectLoader(const char* soPath_) : _path(soPath_) {}
	SharedObjectLoader::~SharedObjectLoader() {}
	void* SharedObjectLoader::getFunctionPointer(const char* funcName_) const { return NULL; }
#endif
	

	bool isValid() const { return (_handle != NULL); }
	string getFileName() const { return _path.stem().generic_string(); }
	string getFullFileName() const { return _path.filename().generic_string(); }
};


#endif //__SHARED_OBJECT_LOADER__H_