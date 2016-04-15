#include "AlgorithmContainer.h"
//#include <dlfcn.h> 

map<string, maker_t*> factory;

AlgorithmContainer::AlgorithmContainer(const char* algorithmPath_)
{
//	void *dlib = dlopen(name, RTLD_NOW);
//	if (dlib == NULL){
//		cerr << dlerror() << endl;
//		exit(-1);
//	}
	_fileName = algorithmPath_;

	// some how load the algo

//	_algorithm = new Algorithm();
	_isValid = false;
	_errorLine = "some error line";
	
}
