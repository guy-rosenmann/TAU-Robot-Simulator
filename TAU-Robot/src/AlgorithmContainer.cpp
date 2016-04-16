#include "AlgorithmContainer.h"
#include <iostream>
#include <dlfcn.h> 
#include <boost/filesystem/path.hpp>

map<string, maker_t*> factory;

AlgorithmContainer::AlgorithmContainer(const char* algorithmPath_)
{
//	cout << "AlgorithmContainer file name :" << algorithmPath_ << endl << std::flush;

	boost::filesystem::path path(algorithmPath_);
	_fileName = path.filename().generic_string();
	_algoName = path.stem().generic_string();

	// Opening the .so file:
	void *dlib = dlopen(algorithmPath_, RTLD_NOW);
	if (dlib == NULL){
//		cout << "error in dlopen:" << endl << std::flush;
//		cout << dlerror() << endl << std::flush;
//		exit(-1);

		_isValid = false;
		_errorLine = _fileName + ": file cannot be loaded or is not a valid .so";
		return;
	}

	typedef AbstractAlgorithm* (*func1_t)();

	// getAbstractFunctionPointer is a function in SumFunction.cpp
	void* p = dlsym(dlib, "getAbstractAlgorithmPointer");
	func1_t function1 = reinterpret_cast<func1_t>(reinterpret_cast<long>(p));

	if (function1 == nullptr) {
		// Setup Error

		_isValid = false;
		_errorLine = _fileName + ": valid .so file but no algorithm was registered after loading it";
		return;
	}

	_algorithm = function1();
	_isValid = true;
}

//map<string, maker_t*> AlgorithmContainer::createFactory()
//{
//	std::cout << "Inside createFactory" << endl << std::flush;
//	map<string, maker_t*> map;
//	return map;
//}


