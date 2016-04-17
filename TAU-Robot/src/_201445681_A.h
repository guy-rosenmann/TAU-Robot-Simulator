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
	const int pseudoDefaultValue = 29;
	unsigned int _psuedoRand = pseudoDefaultValue;

public:
	_201445681_A() {}
	_201445681_A(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){ }

	Direction step();
	void updatePseudoRandom();
	void resetValues();
};


#endif //___201445681_A__H_