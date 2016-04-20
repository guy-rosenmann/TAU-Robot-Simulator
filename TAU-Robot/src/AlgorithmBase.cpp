#include "AlgorithmBase.h"
#include <algorithm>

void AlgorithmBase::aboutToFinish(int stepsTillFinishing_)
{
	_returnHome = true;
	_movesUntilFinish = stepsTillFinishing_;
}

void AlgorithmBase::getPossibleMoves(vector<Direction>& moves_)
{
//	bool returnQuick = false;
	if (_returnHome)
	{
		if (isDocking())
		{
			moves_.push_back(Direction::Stay);
			return;
		}

		vector<Direction>::reverse_iterator rit = _movesDone.rbegin();
		while (rit != _movesDone.rend())
		{
			if ((*rit) == Direction::Stay)
			{
				_movesDone.erase((++rit).base());
			}
			else
			{
				moves_.push_back(oppositeDirection(*rit));
				_movesDone.erase((++rit).base());
				return;
			}
		}
	}

	SensorInformation info = _sensor->sense(); // info.isWall = { East, West, South, North }

	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		if (!info.isWall[i])
		{
			moves_.push_back((Direction)i);
		}
	}

	// Bigger than one because we'll go back the same way
	if (info.dirtLevel > 1 || moves_.size() == 0 || _robot.location == Point())
	{
		// add Stay option only if needed
		// For the algorithm, the docking station is at the point (0,0)
		moves_.push_back(Direction::Stay);
	}

	// No need to go back if we don't have to
	if (!_returnHome && moves_.size() > 1)
	{
		removeBackwardDirection(moves_);
	}
}

void AlgorithmBase::removeBackwardDirection(vector<Direction>& moves_)
{
	// Stay is valid twice for dirty floor
	if (_lastMove == Direction::Stay)
	{
		return;
	}

	vector<Direction>::iterator it = moves_.begin();
	Direction backDir = oppositeDirection(_lastMove);
	while (it != moves_.end())
	{
		Direction& dir = *it;
		if (dir == backDir)
		{
			it = moves_.erase(it);
			return;
		}
		++it;
	}
}
bool AlgorithmBase::isDocking()
{
	return _robot.location == Point();
}

//bool AlgorithmBase::returnQuicklyMoves(vector<Direction>& moves_)
//{
//	vector<Direction> quickReturnMoves;
//	if (_eastLocation > 0)
//	{
//		quickReturnMoves.push_back(Direction::West);
//	}
//	else if (_eastLocation < 0)
//	{
//		quickReturnMoves.push_back(Direction::East);
//	}
//
//	if (_southLocation > 0)
//	{
//		quickReturnMoves.push_back(Direction::North);
//	}
//	else if (_southLocation < 0)
//	{
//		quickReturnMoves.push_back(Direction::South);
//	}
//
//
//	for (vector<Direction>::iterator itQ = quickReturnMoves.begin(); itQ != quickReturnMoves.end(); itQ++)
//	{
//		bool seenQuickReturnMove = false;
//		for (vector<Direction>::iterator it = moves_.begin(); it != moves_.end(); it++)
//		{
//			if ((*it) == (*itQ))
//			{
//				break;
//			}
//			// Reaching here means that the quick move is invalid
//			seen
//		}
//	}
//	
//	
//}

Direction AlgorithmBase::oppositeDirection(Direction direction_)
{
	switch (direction_)
	{
		case Direction::East: return Direction::West;
		case Direction::West: return Direction::East;
		case Direction::North: return Direction::South;
		case Direction::South: return Direction::North;
		case Direction::Stay: return Direction::Stay;
	}
	return Direction::Stay;
}

void AlgorithmBase::updateBeforeMove()
{
//	updateBattery(); // We'll start using this in Ex.3
	updateRemainingMoves();
}

void AlgorithmBase::updateAfterMove(Direction direction_)
{
	// update robot info
	_robot.location.move(direction_);
	_robot.totalSteps++;

	// Save moves
	if (!_returnHome)
	{
		_lastMove = direction_;

		// No need to store stay moves
		if (_lastMove != Direction::Stay)
		{
			_movesDone.push_back(_lastMove);
		}
	}
}

void AlgorithmBase::updateRemainingMoves()
{
	_movesUntilFinish--;
}

void AlgorithmBase::updateBattery()
{
	int consumptionRate = _config["BatteryConsumptionRate"];
	unsigned int rechargeRate = _config["BatteryRechargeRate"];
	unsigned int capacity = _config["BatteryCapacity"];

	if (isDocking())
	{
		_robot.battery = std::max(_robot.battery + rechargeRate, capacity);
	}
	else
	{
		_robot.battery -= -consumptionRate;
	}

	// Next were figuring out if we have enough battery to return home
	if (_returnHome)
	{
		return;
	}

	int returnMoves = _movesDone.size(), returnBatteryConsumption = returnMoves*consumptionRate;
	if ((_robot.battery >= returnBatteryConsumption) && (_robot.battery - consumptionRate <= returnBatteryConsumption))
	{
		_returnHome = true;
	}
}

//void AlgorithmBase::resetValues()
//{
//	_robot.totalSteps = 0;
//	_movesDone.clear();
//	_lastMove = Direction::Stay;
//
//	_returnHome = false;
//	_robot.location = Point();
//
//	_movesUntilFinish = UINT_MAX;
//}
