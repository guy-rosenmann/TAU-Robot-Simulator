#include "AlgorithmContainer.h"

AlgorithmContainer::AlgorithmContainer(const char* algorithmPath_)
{
	_fileName = algorithmPath_;

	// some how load the algo

//	_algorithm = new Algorithm();
	_isValid = false;
	_errorLine = "some error line";
	
}
