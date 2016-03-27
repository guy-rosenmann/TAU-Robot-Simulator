#ifndef __HOUSE_SIMULATION__H_
#define __HOUSE_SIMULATION__H_

#include "Simulation.h"

#include <vector>

using namespace std;


class HouseSimulation
{
	House				_house;
	Configuration		_config;

	vector<Simulation*> _simulations;

public:
	HouseSimulation() = delete;
	HouseSimulation(const Configuration& conf_, const House& house_);
	virtual ~HouseSimulation();

private:
	
};


#endif //__HOUSE_SIMULATION__H_