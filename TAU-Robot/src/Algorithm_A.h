#ifndef __Algorithm_A__H_
#define __Algorithm_A__H_

#include <vector>
#include <map>
using namespace std;

#include "Direction.h"
#include "AlgorithmRegistrar.h"
#include "AlgorithmBase.h"
#include "Configuration.h"
#include "RobotInformation.h"

class Algorithm_A : public AlgorithmBase
{
	unsigned int _psuedoRand = 29;

public:

	Algorithm_A() {}
	Algorithm_A(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){}

	Direction step();
	void updatePseudoRandom();
};


#endif //__Algorithm_A__H_