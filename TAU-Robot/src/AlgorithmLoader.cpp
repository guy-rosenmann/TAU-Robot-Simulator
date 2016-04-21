#include "AlgorithmLoader.h"
#include <iostream>
#include <boost/filesystem.hpp>

#ifndef _WINDOWS_
#include <dlfcn.h>
#endif


map<string, instanceCreator> globalFactory;

#ifndef _WINDOWS_
AlgorithmLoader::AlgorithmLoader(const char* algorithmPath_)
{
	boost::filesystem::path path(algorithmPath_);
	_fileName = path.filename().generic_string();
	_algoName = path.stem().generic_string();

	// Opening the .so file:
	_handle = dlopen(algorithmPath_, RTLD_NOW);
	if (_handle == NULL)
	{
		_errorLine = _fileName + ": file cannot be loaded or is not a valid .so";
		return;
	}

	// getAbstractAlgorithmPointer is the instance creator method
	void* p = dlsym(_handle, "getAbstractAlgorithmPointer");

	// Safe casting
	instanceCreator function1 = reinterpret_cast<instanceCreator>(reinterpret_cast<long>(p));

	if (function1 == nullptr) {
		_errorLine = _fileName + ": valid .so file but no algorithm was registered after loading it";
		return;
	}

	globalFactory[_algoName] = function1;

	_isValid = true;
}

AlgorithmLoader::~AlgorithmLoader()
{
	if (_handle != NULL)
	{
		dlclose(_handle);
	}
}

#else
// loading in Windows without shared objects - only for tests!
AlgorithmLoader::AlgorithmLoader(AbstractAlgorithm* algo_, const char* algoName_)
{
	_isValid = true;
	
}
#endif