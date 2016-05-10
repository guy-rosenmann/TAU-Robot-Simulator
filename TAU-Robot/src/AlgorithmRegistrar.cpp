#include "AlgorithmRegistrar.h"

AlgorithmRegistrar AlgorithmRegistrar::_instance;


#ifdef _WINDOWS_
// for Windows tests only
// Add all algorithms manually
#include "MakeUnique.h"
#include "201445681_A_.h"
#include "201445681_B_.h"
#include "201445681_C_.h"

#define WIN_WRITE_ALGO(name) \
	_instance._algorithmPairs.push_back( \
		std::make_pair(new SharedObjectLoader(#name "_.so"), []{return make_unique<_##name>(); }) \
);

AlgorithmRegistrar::AlgorithmRegistrar()
{
	WIN_WRITE_ALGO(201445681_A);
//	WIN_WRITE_ALGO(201445681_B);
//	WIN_WRITE_ALGO(201445681_C);
}
#endif


AlgorithmRegistrar::~AlgorithmRegistrar()
{
	vector<SharedObjectLoader*> deleteList;

	// need to clear lambda expressions before unloading the shared object
	for (vector<AlgoLoaderPair>::iterator it = _algorithmPairs.begin(); it != _algorithmPairs.end(); ++it)
	{
		deleteList.push_back(it->first);
	}
	_algorithmPairs.clear();

	for (vector<SharedObjectLoader*>::iterator it = deleteList.begin(); it != deleteList.end(); ++it)
	{
		delete *it;
	}
}


int AlgorithmRegistrar::loadAlgorithm(const char* soPath_)
{
	SharedObjectLoader* loader = new SharedObjectLoader(soPath_);

	if (!loader->isValid())
	{
		delete loader;
		return FILE_CANNOT_BE_LOADED;
	}
	else if (_algorithmPairs.size() == 0 || _algorithmPairs.back().first != nullptr)
	{
		delete loader;
		return NO_ALGORITHM_REGISTERED;
	}

	AlgoLoaderPair& newAlgo = _algorithmPairs.back();
	newAlgo.first = loader;

	return ALGORITHM_REGISTERED_SUCCESSFULY;
}


map<string, unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const
{
	map<string, unique_ptr<AbstractAlgorithm>> algorithms;
	for (vector<AlgoLoaderPair>::const_iterator it = _algorithmPairs.begin(); it != _algorithmPairs.end(); ++it)
	{
		SharedObjectLoader& loader = *(it->first);
		algorithms[loader.getFileName()] = (it->second)();
	}

	return algorithms;
}


vector<string> AlgorithmRegistrar::getAlgorithmNames() const
{
	vector<string> names;
	for (vector<AlgoLoaderPair>::const_iterator it = _algorithmPairs.begin(); it != _algorithmPairs.end(); ++it)
	{
		SharedObjectLoader& loader = *(it->first);
		names.push_back(loader.getFileName());
	}

	return names;
}


void AlgorithmRegistrar::registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
	_instance._algorithmPairs.push_back(std::make_pair(nullptr, algorithmFactory));
}