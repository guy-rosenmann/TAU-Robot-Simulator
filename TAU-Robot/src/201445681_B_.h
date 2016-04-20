#ifndef ___201445681_B__H_
#define ___201445681_B__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "Configuration.h"
#include "RobotInformation.h"
#include "AlgorithmBase.h"

class _201445681_B : public AlgorithmBase
{
	unsigned int _psuedoRand = 30;

public:
	_201445681_B() {}
	_201445681_B(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf) {}

	Direction step();
	void updatePseudoRandom();
};


#endif //___201445681_B__H_