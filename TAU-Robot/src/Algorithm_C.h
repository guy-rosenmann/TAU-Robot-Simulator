#ifndef __Algorithm_C__H_
#define __Algorithm_C__H_

#include <vector>
#include <map>
using namespace std;


#include "Direction.h"
#include "AlgorithmRegistrar.h"
#include "Configuration.h"
#include "RobotInformation.h"
#include "AlgorithmBase.h"

class Algorithm_C : public AlgorithmBase
{
	unsigned int _psuedoRand = 31;

public:

	Algorithm_C() {}
	Algorithm_C(const AbstractSensor& sensor, const Configuration& conf) : AlgorithmBase(sensor, conf){}

	Direction step();

};


#endif //__Algorithm_C__H_