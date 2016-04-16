#ifndef __ALGORITHM_BASE__H_
#define __ALGORITHM_BASE__H_

#include <vector>
#include <map>
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

	void setSensor(const AbstractSensor& sensor) { _sensor = &sensor; }
	void setConfiguration(map<string, int> config) { _config = config; }

	virtual Direction step() = 0;

	void aboutToFinish(int stepsTillFinishing);

protected:
	const AbstractSensor*	_sensor = nullptr;
	map<string, int>		_config;
	RobotInformation		_robot;

	Direction _lastMove = Direction::Stay;
	bool _returnHome = false;
	vector<Direction> _movesDone;
	int _southLocation = 0;
	int _eastLocation = 0;

	bool isDocking();
//	bool returnQuicklyMoves(vector<Direction>& moves_);
	void getPossibleMoves(vector<Direction>& moves_);
	static Direction oppositeDirection(Direction direction_);
	void removeBackwardDirection(vector<Direction>& moves_);
	void updateLocation(Direction direction_);
};


#endif //__ALGORITHM_BASE__H_