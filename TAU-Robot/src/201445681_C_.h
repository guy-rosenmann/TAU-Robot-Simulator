#ifndef ___201445681_C__H_
#define ___201445681_C__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "Configuration.h"
#include "RobotInformation.h"
#include "AlgorithmBase.h"

class _201445681_C : public AlgorithmBase
{
	vector<Direction> _myOrder;

public:
	_201445681_C();
	_201445681_C(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf) {}

	Direction step(Direction prevStep);
	void updatePseudoRandom();
};


#endif //___201445681_C__H_