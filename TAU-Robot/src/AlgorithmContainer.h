#ifndef __ALGORITHMCONTAINER__H_
#define __ALGORITHMCONTAINER__H_

#include "Direction.h"
#include "AbstractAlgorithm.h"

class AlgorithmContainer
{
	AbstractAlgorithm*		_algorithm;

	bool					_isValid;
	string					_fileName;
	string					_errorLine;

public:

	AlgorithmContainer() = delete;
	AlgorithmContainer(const char* algorithmPath_);

	bool isValid() const { return _isValid; }
	string getFileName() const { return _fileName; }
	string getErrorLine() const { return _errorLine; }
	AbstractAlgorithm* getAlgorithm() const { return _algorithm; }

};


#endif //__ALGORITHMCONTAINER__H_