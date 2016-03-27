#ifndef __SENSOR__H_
#define __SENSOR__H_

#include "AbstractSensor.h"


class Sensor : public AbstractSensor
{
	friend class Simulation;

	SensorInformation _info;

public:
	SensorInformation sense() const { return _info; }

};

#endif //__SENSOR__H_