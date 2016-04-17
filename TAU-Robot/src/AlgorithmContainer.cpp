#include "AlgorithmContainer.h"
#include <iostream>
#include <dlfcn.h> 
#include <boost/filesystem/path.hpp>

AlgorithmContainer::AlgorithmContainer(const char* algorithmPath_)
{
	// REMOVE
//	cout << "Starting AlgorithmContainer" << endl;

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

	typedef AbstractAlgorithm* (*func1_t)();

	// getAbstractAlgorithmPointer is the instance creator method
	void* p = dlsym(dlib, "getAbstractAlgorithmPointer");

	// Safe casting
	func1_t function1 = reinterpret_cast<func1_t>(reinterpret_cast<long>(p));

	if (function1 == nullptr) {
		_isValid = false;
		_errorLine = _fileName + ": valid .so file but no algorithm was registered after loading it";
		return;
	}

	_algorithm = function1();
	_isValid = true;
}

AlgorithmContainer::~AlgorithmContainer()
{
	// REMOVE
//	cout << "Starting ~AlgorithmContainer" << endl;

	delete _algorithm;

	// REMOVE
//	cout << "Finishing ~AlgorithmContainer" << endl;
}


