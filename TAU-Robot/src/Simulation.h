#ifndef __SIMULATION__H_
#define __SIMULATION__H_

#include "RobotInformation.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Sensor.h"
#include "Configuration.h"


class Simulation
{
	AbstractAlgorithm*	_algo = nullptr;
	House				_house;
	Sensor				_sensor;
	RobotInformation	_robot;
	Configuration		_config;

public:

	Simulation() = delete;
	Simulation(const Configuration& config_, const House& house_, AbstractAlgorithm* algo_);
	virtual ~Simulation();

	bool step();
	int getStepsCount() const { return _robot.totalSteps; }
	int getTotalDirtCount() const { return _house.getTotalDirtAmount(); }
	int getCleanedDirtCount() const { return _robot.cleanedDirt; }
	bool isRobotDocked() const { return (_robot.location == _house.getDocking()); }
	bool isRobotOutOfBattery() const { return _robot.stuck; }
	bool didRobotMisbehave() const { return !_robot.goodBehavior; }
	bool isDone() const;
	void printStatus();
	void CallAboutToFinish(int stepsTillFinishing);

	int score(int position_in_competition, int winner_num_steps, int SimulationSteps) const;

	RobotInformation getRobotInfo() const { return _robot; }

	// smaller = done with less steps
	bool operator<(const Simulation& other) const;
	bool operator>(const Simulation& other) const;

private:
	void updateSensor();
};


#endif //__SIMULATION__H_