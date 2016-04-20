#include "AlgorithmLoader.h"
#include <iostream>
#include <dlfcn.h> 
#include <boost/filesystem/path.hpp>

map<string, instanceCreator> globalFactory;

AlgorithmLoader::AlgorithmLoader(const char* algorithmPath_)
{
	boost::filesystem::path path(algorithmPath_);
	_fileName = path.filename().generic_string();
	_algoName = path.stem().generic_string();

	// Opening the .so file:
	void *dlib = dlopen(algorithmPath_, RTLD_NOW);
	if (dlib == NULL){

		_isValid = false;
		_errorLine = _fileName + ": file cannot be loaded or is not a valid .so";
		return;
	}

	// getAbstractAlgorithmPointer is the instance creator method
	void* p = dlsym(dlib, "getAbstractAlgorithmPointer");

	// Safe casting
	instanceCreator function1 = reinterpret_cast<instanceCreator>(reinterpret_cast<long>(p));

	if (function1 == nullptr) {
		_isValid = false;
		_errorLine = _fileName + ": valid .so file but no algorithm was registered after loading it";
		return;
	}

	globalFactory[_algoName] = function1;
	_isValid = true;
}

