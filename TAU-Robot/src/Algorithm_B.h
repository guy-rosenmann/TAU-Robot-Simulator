#ifndef __Algorithm_B__H_
#define __Algorithm_B__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "AlgorithmRegistrar.h"
#include "Configuration.h"
#include "RobotInformation.h"
#include "AlgorithmBase.h"

class Algorithm_B : public AlgorithmBase
{
	unsigned int _psuedoRand = 30;

public:

	Algorithm_B() {}
	Algorithm_B(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){}

	Direction step();
};


#endif //__Algorithm_B__H_