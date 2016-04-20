#ifndef ___201445681_A__H_
#define ___201445681_A__H_

#include <vector>
#include <map>
using namespace std;

#include "Direction.h"
#include "AlgorithmBase.h"
#include "Configuration.h"
#include "RobotInformation.h"

class _201445681_A : public AlgorithmBase
{
	unsigned int _psuedoRand = 29;

public:
	_201445681_A() {}
	_201445681_A(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){ }

	Direction step();
	void updatePseudoRandom();
};


#endif //___201445681_A__H_