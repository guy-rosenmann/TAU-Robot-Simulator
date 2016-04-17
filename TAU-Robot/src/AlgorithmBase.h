#ifndef __ALGORITHM_BASE__H_
#define __ALGORITHM_BASE__H_

#include <vector>
#include <map>
#include <limits.h>
using namespace std;


#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Configuration.h"
#include "RobotInformation.h"

class AlgorithmBase : public AbstractAlgorithm
{

public:

	AlgorithmBase() {}
	AlgorithmBase(const AbstractSensor& sensor, const Configuration& conf) { setSensor(sensor); setConfiguration(conf.getParams()); }

	void setSensor(const AbstractSensor& sensor) { _sensor = &sensor; resetValues(); }
	void setConfiguration(map<string, int> config) { _config = config; _robot.battery = _config["BatteryConsumptionRate"]; }

	virtual Direction step() = 0;
	void aboutToFinish(int stepsTillFinishing);

	void updateBattery();
	virtual void resetValues();

protected:
	const AbstractSensor*	_sensor = nullptr;
	map<string, int>		_config;
	RobotInformation		_robot;

	Direction _lastMove = Direction::Stay;
	bool _returnHome = false;
	vector<Direction> _movesDone;
	unsigned int _movesUntilFinish = UINT_MAX;
	
	bool isDocking();
	void getPossibleMoves(vector<Direction>& moves_);
	static Direction oppositeDirection(Direction direction_);
	void removeBackwardDirection(vector<Direction>& moves_);
	void updateRemainingMoves();
	void updateBeforeMove();
	void updateAfterMove(Direction direction_);
};


#endif //__ALGORITHM_BASE__H_