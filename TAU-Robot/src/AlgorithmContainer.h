#ifndef __ALGORITHMCONTAINER__H_
#define __ALGORITHMCONTAINER__H_

#include "Direction.h"
#include "AbstractAlgorithm.h"

class AlgorithmContainer
{
	AbstractAlgorithm*		_algorithm = nullptr;

	bool					_isValid;
	string					_fileName;
	string					_algoName;
	string					_errorLine;

public:

	AlgorithmContainer() = delete;
	AlgorithmContainer(const char* algorithmPath_);
	AlgorithmContainer(AbstractAlgorithm* algorithm_, const char* algoName_) : _algorithm(algorithm_), _algoName(algoName_){}
	~AlgorithmContainer();

	bool isValid() const { return _isValid; }
	string getFileName() const { return _fileName; }
	string GetAlgorithmName() const { return _algoName; }
	string getErrorLine() const { return _errorLine; }
	AbstractAlgorithm* getAlgorithm() const { return _algorithm; }

};


#endif //__ALGORITHMCONTAINER__H_