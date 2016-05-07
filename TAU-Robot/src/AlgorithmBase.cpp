#include "AlgorithmBase.h"
#include <algorithm>

AlgorithmBase::AlgorithmBase()
{
	_house = new char*[MAXHOUSELENGTH];
	for (size_t i = 0; i < MAXHOUSELENGTH; i++)
	{
		_house[i] = new char[MAXHOUSELENGTH + 1];
		_house[i][MAXHOUSELENGTH] = '\0';
		std::memset(_house[i], AlgorithmBase::UNKNOWN, MAXHOUSELENGTH); // fill in all places with spaces
	}

	_robot.location = Point(MAXHOUSELENGTH / 2, MAXHOUSELENGTH / 2);

}

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

void AlgorithmBase::removeBackwardDirection(vector<Direction>& moves_) const
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
bool AlgorithmBase::isDocking() const
{
	return _robot.location == Point(INITIALLENGTH, INITIALLENGTH);
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

void AlgorithmBase::updateBeforeMove(Direction prevMove_) const
{

}

void AlgorithmBase::updateAfterMove(Direction direction_)
{
	// update robot info
	_robot.location.move(direction_);
	_robot.totalSteps++;
//	_house[];

	updateBattery();
	updateRemainingMoves();

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

Direction AlgorithmBase::recoverFromUndisciplinedRobot(Direction actualPrevStep_) const
{
	return oppositeDirection(actualPrevStep_);


//	if (_mode == UNDISCIPLINED)
//	{
//		_mode = UNDISCIPLINED2;
//	}
//	else
//	{
//		_prevMode = _mode;
//		_mode = UNDISCIPLINED;
//	}
//
//
//	if (actualPrevStep_ == Direction::Stay)
//	{
//		_mode = _prevMode;
//		return _lastMove;
//	}
//
//	if (_lastMove == Direction::Stay)
//	{
//		_mode = _prevMode;
//		return oppositeDirection(actualPrevStep_);
//	}
//
//	// if we got here, we'll need two steps to recover
//	if (_mode == UNDISCIPLINED2)
//	{
//		_mode = _prevMode;
//		return oppositeDirection(actualPrevStep_);
//	}


}

string AlgorithmBase::DirectionToString(Direction direction) const
{
	switch (direction)
	{
	case Direction::Stay: return "stay";
	case Direction::East: return "east";
	case Direction::West: return "west";
	case Direction::North: return "north";
	case Direction::South: return "south";
	}
}

void AlgorithmBase::updateHouseKnowladge(SensorInformation info)
{
	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		Direction direction = (Direction) i;
		Point block = _robot.location;

#ifdef _DEBUG_
		cout << "Block before move:" << block << endl;
		cout << "move direction:" << DirectionToString(direction) << endl;
#endif
		block.move(direction);

#ifdef _DEBUG_
		cout << "Block after move:" << block << endl;
#endif

		if (_house[block.getY()][block.getX()] == UNKNOWN)
		{
			if (info.isWall[i])
			{
				_house[block.getY()][block.getX()] = WALL;
			}
			else
			{
				_house[block.getY()][block.getX()] = NOTWALL;
				_NLocations.insert(block);
			}
		}
	}

	int dL = info.dirtLevel;
	_NLocations.erase(_robot.location);
	_house[_robot.location.getY()][_robot.location.getX()] = (dL == 0) ? EMPTY : CLEAN + dL;
}

Direction AlgorithmBase::getMove(Direction prevStep_)
{
	if (prevStep_ != _lastMove && prevStep_ != Direction::Stay)
	{
		return recoverFromUndisciplinedRobot(prevStep_);
	}

	SensorInformation info = _sensor->sense(); // info.isWall = { East, West, South, North }
	updateHouseKnowladge(info);

	vector<Direction> possibleMoves;
	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		if (!info.isWall[i])
		{
			possibleMoves.push_back((Direction)i);
		}
	}

	// battery Check

	if (_mode == SCAN && info.dirtLevel >= 1)
	{
		return Direction::Stay;
	}




	return possibleMoves[rand() % possibleMoves.size()];
}

Direction AlgorithmBase::goToPoint(Point destination)
{

}

void AlgorithmBase::printHouse(Point robotLocation) const
{
	cout << "####################################################" << endl;
	cout << "Guessed House : " << endl;
	char prev = _house[robotLocation.getY()][robotLocation.getX()];
	_house[robotLocation.getY()][robotLocation.getX()] = 'R';
	for (int i = 0; i < MAXHOUSELENGTH; i++)
	{
		cout << _house[i] << endl;
	}
	_house[robotLocation.getY()][robotLocation.getX()] = prev;
	cout << "####################################################" << endl;
}

void AlgorithmBase::printNLocation()
{
	cout << "Printing Set:" << endl;
	for (auto iter = _NLocations.begin(); iter != _NLocations.end(); ++iter){
		cout<<(*iter) <<endl;
	}
}

void AlgorithmBase::updateRemainingMoves()
{
	_movesUntilFinish--;
}

size_t AlgorithmBase::NumberOfMovesToDocking() const
{
	return _movesDone.size();
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
		_robot.battery -= consumptionRate;
	}

	// Next were figuring out if we have enough battery to return home
	if (_returnHome)
	{
		return;
	}

	int returnBatteryConsumption = NumberOfMovesToDocking()*consumptionRate;

	// The 2 marked here is a safety margin //////////////////////////   -|-  /////////////////////
	if ((_robot.battery >= returnBatteryConsumption) && (_robot.battery - 2 * consumptionRate <= returnBatteryConsumption))
	{
		_returnHome = true;
	}
}
