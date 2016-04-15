#ifndef __Algorithm_B__H_
#define __Algorithm_B__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Configuration.h"
#include "RobotInformation.h"

class Algorithm_B : public AbstractAlgorithm
{

	const AbstractSensor*	_sensor = nullptr;
	const unsigned int		_psuedoRand = 30;
	map<string, int>		_config;
	RobotInformation		_robot;

public:

	Algorithm_B() {}
	Algorithm_B(const AbstractSensor& sensor, const Configuration& conf) { setSensor(sensor); setConfiguration(conf.getParams()); }

	void setSensor(const AbstractSensor& sensor) { _sensor = &sensor; }
	void setConfiguration(map<string, int> config) { _config = config; }
	void aboutToFinish(int stepsTillFinishing) {} // TODO: implement for EX2

	Direction step();

private:
	void getPossibleMoves(vector<Direction>& moves_);

};


#endif //__Algorithm_B__H_