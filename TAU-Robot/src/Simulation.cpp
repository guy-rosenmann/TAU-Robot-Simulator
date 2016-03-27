#include "Simulation.h"

#include <algorithm>

Simulation::Simulation(const Configuration& config_, const House& house_, AbstractAlgorithm* algo_) : _house(house_), _algo(algo_), _config(config_)
{
	_robot.battery = _config["BatteryCapacity"];
	_robot.location = _house.getDocking();

	_config = config_;
	_house = house_;
	_algo = algo_;

	this->updateSensor();
	
	_algo->setConfiguration(_config.getParams());
	_algo->setSensor(_sensor);
}

Simulation::~Simulation()
{
	//delete _algo; // currently freed by Simulator
}

bool Simulation::step()
{
	int capacity = _config["BatteryCapacity"],
		rechargeRate = _config["BatteryRachargeRate"],
		consumptionRate = _config["BatteryConsumptionRate"];	

	Direction stepDirection = _algo->step();
	_robot.totalSteps++;

	if (stepDirection == Direction::Stay)
	{
		if (_robot.location == _house.getDocking())
		{
			_robot.battery = std::min(capacity, _robot.battery + rechargeRate);
		}
		else
		{
			if (_robot.battery - consumptionRate < 0)
			{
				_robot.stuck = true;
				return false;
			}

			_robot.cleanedDirt += _house.clean(_robot.location);
			_robot.battery -= consumptionRate;
		}
	}
	else
	{
		if (_robot.battery - consumptionRate >= 0)
		{
			Point destPoint = _robot.location;
			destPoint.move(stepDirection);

			char destType = _house.at(destPoint);
			if (destType == House::ERR || destType == House::WALL)
			{
				_robot.goodBehavior = false;
				return false; // outside the house / into a wall
			}
			else
			{
				_robot.cleanedDirt += _house.clean(_robot.location);
				_robot.battery -= consumptionRate;
				_robot.location.move(stepDirection);
			}
		}
		else
		{
			_robot.stuck = true;
			return false;
		}
	}

	this->updateSensor();
	return true;
}


bool Simulation::isDone() const
{
	return _house.isClean() && (_robot.location == _house.getDocking());
}


int Simulation::score(int position_in_copmetition, int winner_num_steps, int SimulationSteps) const
{
	std::cout << _house;
	std::cout << "Clean = " << _robot.cleanedDirt << endl;
	if (!_robot.goodBehavior)
	{
		return 0;
	}

	int this_num_steps = _robot.stuck ? SimulationSteps : this->getStepsCount();

	int points = 2000;
	points -= (position_in_copmetition - 1) * 50;
	points += (winner_num_steps - this_num_steps) * 10;
	points -= (this->getTotalDirtCount() - this->getCleanedDirtCount()) * 3;
	points += this->isRobotDocked() ? 50 : -200;

	return std::max(0, points);
}


bool Simulation::operator<(const Simulation& other) const
{
	bool thisDone = this->isDone(), otherDone = other.isDone();
	if (otherDone == thisDone)
	{
		return (_robot.totalSteps < other._robot.totalSteps);
	}
	return (thisDone && !otherDone);
}


bool Simulation::operator>(const Simulation& other) const
{
	bool thisDone = this->isDone(), otherDone = other.isDone();
	if (otherDone == thisDone)
	{
		return (_robot.totalSteps > other._robot.totalSteps);
	}
	return (!thisDone && otherDone);
}

void Simulation::updateSensor()
{
	char state = _house.at(_robot.location);
	if (state >= '1' && state <= '9')
	{
		_sensor._info.dirtLevel = (int)(state - '0');
	}
	else
	{
		_sensor._info.dirtLevel = 0;
	}

	for (int direction = (int)Direction::East; direction < (int)Direction::Stay; ++direction)
	{
		Point temp = _robot.location;
		temp.move((Direction)direction);
		_sensor._info.isWall[direction] = (_house.at(temp) == House::WALL);
	}
}