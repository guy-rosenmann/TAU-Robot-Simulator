#ifndef __ALGORITHM_REGISTRAR__H_
#define __ALGORITHM_REGISTRAR__H_

#include <functional>
#include <memory>
#include <vector>
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "SharedObjectLoader.h"

using namespace std;

class AlgorithmRegistrar
{
	static AlgorithmRegistrar _instance;
	
	typedef std::pair<SharedObjectLoader*, std::function<unique_ptr<AbstractAlgorithm>()>> AlgoLoaderPair;
	std::vector<AlgoLoaderPair> _algorithmPairs;

public:
	friend class AlgorithmRegistration;

	enum { ALGORITHM_REGISTERED_SUCCESSFULY = 0, FILE_CANNOT_BE_LOADED = -1, NO_ALGORITHM_REGISTERED = -2 };
	
#ifdef _WINDOWS_
	// for Windows tests only
	AlgorithmRegistrar();
#endif
	~AlgorithmRegistrar();
	
	int loadAlgorithm(const char* soPath_);
	map<string, unique_ptr<AbstractAlgorithm>> getAlgorithms() const;
	vector<string> getAlgorithmNames() const;
	size_t size() const { return _algorithmPairs.size(); }
	
	static AlgorithmRegistrar& getInstance() { return _instance; }

private:

	void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory);
};

#endif //__ALGORITHM_REGISTRAR__H_