#include "Simulation.h"

#include <algorithm>

Simulation::Simulation(const Configuration& config_, const House& house_, unique_ptr<AbstractAlgorithm>& algo_, string algoName_) : _algoName(algoName_), _house(house_), _config(config_)
{
	_robot.battery = _config["BatteryCapacity"];
	_robot.location = _house.getDocking();

	_config = config_;
	_house = house_;
	_algo = algo_.release();

	this->updateSensor();
	
	_algo->setConfiguration(_config.getParams());
	_algo->setSensor(_sensor);
}

Simulation::~Simulation()
{
	delete _algo;
}

bool Simulation::step()
{
	if (_robot.battery <= 0)
	{
#ifdef _DEBUG_
		// for EX1: printing after score!
		// cout << "[INFO] The robot is stuck with an empty battery." << endl;
#endif
		_robot.stuck = true;
		return false;
	}
	
	
	int capacity = _config["BatteryCapacity"],
		rechargeRate = _config["BatteryRechargeRate"],
		consumptionRate = _config["BatteryConsumptionRate"];	
	
	
	// if the robot starts from docking station - charge battery (even if leaves)
	if (_house.at(_robot.location) == House::DOCKING)
	{
		_robot.battery = std::min(capacity, _robot.battery + rechargeRate);
	}
	else
	{
		_robot.battery -= consumptionRate;
	}

	// always make a move if battery is larger than 0 at the beggining
	Direction stepDirection = _algo->step(_prevStep);
	_prevStep = stepDirection;

	_robot.location.move(stepDirection);
	_robot.totalSteps++;

	char nextType = _house.at(_robot.location);
	if (nextType == House::ERR || nextType == House::WALL)
	{
#ifdef _DEBUG_
		// for EX1: printing after score!
//		 cout << "[INFO] The robot is trying to walk through a wall." << endl;
#endif
		_robot.goodBehavior = false;
		return false; // outside the house / into a wall
	}

	_robot.cleanedDirt += _house.clean(_robot.location);
	this->updateSensor();

	return true;
}


bool Simulation::isDone() const
{
	return _house.isClean() && (_robot.location == _house.getDocking());
}


void Simulation::printStatus()
{
	for (int i = 0; i < 100; ++i) cout << "#";
	cout << endl << endl;;

	cout << "Steps count: " << _robot.totalSteps << endl;
	cout << "Robot status:" << endl;
	cout << "- Battery: " << _robot.battery << endl;
	cout << "- Dirt Collected: " << _robot.cleanedDirt << endl << endl;
	cout << "House: " << endl;
	_house.print(_robot.location);
	cout << endl;

	for (int i = 0; i < 100; ++i) cout << "#";
	cout << endl;
}

void Simulation::CallAboutToFinish(int stepsTillFinishing)
{
	_algo->aboutToFinish(stepsTillFinishing);
}

int Simulation::score(int actual_position_in_competition, int winner_num_steps, int simulation_steps) const
{
	map<string, int> scoreParams;
	scoreParams["actual_position_in_competition"] = actual_position_in_competition;
	scoreParams["simulation_steps"] = simulation_steps;
	scoreParams["winner_num_steps"] = winner_num_steps;
	scoreParams["this_num_steps"] = this->isRobotOutOfBattery() ? simulation_steps : this->getStepsCount();
	scoreParams["sum_dirt_in_house"] = this->getTotalDirtCount();
	scoreParams["dirt_collected"] = this->getCleanedDirtCount();
	scoreParams["is_back_in_docking"] = this->isRobotDocked() ? 1 : 0;

	return Simulation::calc_score(scoreParams);
}


int Simulation::calc_score(const map<string, int>& score_params_)
{
	bool isHouseClean = score_params_.at("sum_dirt_in_house") == score_params_.at("dirt_collected");
	bool isDocked = score_params_.at("is_back_in_docking") == 1 ? true : false;
	bool isDone = isHouseClean && isDocked;

	int position_in_competition = isDone ? std::min(score_params_.at("actual_position_in_competition"), 4) : 10;

	int points = 2000;
	points -= (position_in_competition - 1) * 50;
	points += (score_params_.at("winner_num_steps") - score_params_.at("this_num_steps")) * 10;
	points -= (score_params_.at("sum_dirt_in_house") - score_params_.at("dirt_collected")) * 3;
	points += isDocked ? 50 : -200;

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

bool Simulation::Compare(const Simulation* simu, const Simulation* other)
{
	return (*simu)<(*other);
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