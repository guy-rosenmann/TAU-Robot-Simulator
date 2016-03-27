#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <vector>
#include <string>

using namespace std;

#include "Simulation.h"


class Simulator
{
	Configuration				_config;
	vector<House*>				_houses;
	
	typedef std::pair<AbstractAlgorithm*, std::vector<int>*> AlgoPair;
	typedef std::vector< AlgoPair > AlgoVector;
	AlgoVector					_algos;

public:
	Simulator() = delete;
	Simulator(const Configuration& conf_, const char* housePath_ = NULL);
	~Simulator();

	void simulate();

private:
	void score(int simulationSteps, vector<Simulation*>& simulatios_);
	int getPosition(const Simulation& sim_) const { return 1; }

	
	template <class T>
	static void clearPointersVector(vector<T*>& vec);

};


#endif //__SIMULATOR__H_