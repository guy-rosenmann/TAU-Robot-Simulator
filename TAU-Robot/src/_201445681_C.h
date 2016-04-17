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
	const int pseudoDefaultValue = 31;
	unsigned int _psuedoRand = pseudoDefaultValue;

public:
	_201445681_C() {}
	_201445681_C(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){}

	Direction step();
	void updatePseudoRandom();
	void resetValues();
};


#endif //___201445681_C__H_