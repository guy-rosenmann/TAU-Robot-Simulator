#ifndef __ALGORITHM__H_
#define __ALGORITHM__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Configuration.h"

class Algorithm : public AbstractAlgorithm
{
	
	const AbstractSensor*	_sensor = nullptr;
	map<string, int>		_config;
	
public:

	Algorithm() {}
	Algorithm(const AbstractSensor& sensor, const Configuration& conf) { setSensor(sensor); setConfiguration(conf.getParams()); }

	void setSensor(const AbstractSensor& sensor) { _sensor = &sensor; }
	void setConfiguration(map<string, int> config) { _config = config; }
	void aboutToFinish(int stepsTillFinishing) {} // TODO: implement for EX2

	Direction step();

private:
	void getPossibleMoves(vector<Direction>& moves_);

};


#endif //__ALGORITHM__H_