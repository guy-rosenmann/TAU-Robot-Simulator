#include "HouseSimulation.h"
#include "Algorithm.h"

HouseSimulation::HouseSimulation(const Configuration& conf_, const House& house_)
{
	_house = house_;
	_config = conf_;
	
	AbstractAlgorithm* algos[] = {
		new Algorithm()
	};
	int numOfAlgos = sizeof(algos) / sizeof(AbstractAlgorithm*);

	for (int i = 0; i < numOfAlgos; ++i)
	{
		_simulations.push_back(new Simulation(_config, _house, algos[i]));
	}
}

HouseSimulation::~HouseSimulation()
{
	for (vector<Simulation*>::iterator it = _simulations.begin(); it != _simulations.end(); ++it)
	{
		delete *it;
	}
}