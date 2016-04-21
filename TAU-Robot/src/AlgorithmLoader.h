#ifndef __AlgorithmLoader__H_
#define __AlgorithmLoader__H_

#include "Direction.h"
#include "AlgorithmRegistrar.h"


class AlgorithmLoader
{
	void*					_handle = NULL;
	bool					_isValid = false;
	string					_fileName;
	string					_algoName;
	string					_errorLine;

public:

	AlgorithmLoader() = delete;
#ifndef _WINDOWS_
	AlgorithmLoader(const char* algorithmPath_);
	~AlgorithmLoader();
#else
	AlgorithmLoader(AbstractAlgorithm* algo_, const char* algoName_);
	~AlgorithmLoader() {}
#endif
	

	bool isValid() const { return _isValid; }
	string getFileName() const { return _fileName; }
	string GetAlgorithmName() const { return _algoName; }
	string getErrorLine() const { return _errorLine; }
};


#endif //__AlgorithmLoader__H_