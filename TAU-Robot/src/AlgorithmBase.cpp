#include "AlgorithmBase.h"

#include <algorithm>
#include <cstring>
#include <queue>

AlgorithmBase::AlgorithmBase()
{
	_house = new char*[MAXHOUSELENGTH];
	for (size_t i = 0; i < MAXHOUSELENGTH; i++)
	{
		_house[i] = new char[MAXHOUSELENGTH + 1];
		_house[i][MAXHOUSELENGTH] = '\0';
		memset(_house[i], AlgorithmBase::UNKNOWN, MAXHOUSELENGTH); // fill in all places with unknowns
	}

	_robot.location = Point(_houseLength / 2, _houseHeight / 2);
	_docking = Point(_houseLength / 2, _houseHeight / 2);

}

void AlgorithmBase::aboutToFinish(int stepsTillFinishing_)
{
	_aboutToFinishCalled = true;
	_movesUntilFinish = stepsTillFinishing_;
}
	
void AlgorithmBase::dijakstra(Point dest_, vector<Direction>& result_){
	queue<Point> queue;
	set<Point> untouched;
	map<Point, Direction> map;

	for (size_t i = 0; i < _houseHeight; ++i)
	{
		for (size_t j = 0; j < _houseLength; ++j)
		{
			char block = _house[i][j];
			if (block != UNKNOWN && block != WALL)
			{
				untouched.insert(Point(j, i));
			}
		}
	}

	queue.push(_robot.location);
	untouched.erase(_robot.location);

	bool foundPath = false;
	while (!foundPath && queue.size() > 0)
	{
		Point point = queue.front();
		queue.pop();

		for (int i = 0; i < 4; i++)
		{
			Point block = point;
			block.move((Direction)i);

			if (untouched.find(block) != untouched.end())
			{
				untouched.erase(block);
				queue.push(block);
				
				map[block] = ((Direction) i);
			}

			if (block == dest_)
			{
				foundPath = true;
			}
		}
	}

	if (!foundPath)
	{
		// nothing to do
		return;
	}

	// storing path to destination
	result_.clear();
	Point p = dest_;

	while (!(p == _robot.location))
	{
		Direction dir = map[p];
		result_.push_back(dir);
		p.move(oppositeDirection(dir));
	}
}

void AlgorithmBase::getPossibleMoves(vector<Direction>& moves_)
{
//	bool returnQuick = false;
	if (_aboutToFinishCalled)
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
	if (!_aboutToFinishCalled && moves_.size() > 1)
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
	return _robot.location == _docking;
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

void AlgorithmBase::updateBeforeMove(Direction choosenMove_)
{
	_lastMove = choosenMove_;
}

void AlgorithmBase::updatePoints(unsigned int xOffset, unsigned int yOffset)
{
	_robot.location = Point(_robot.location.getX() + xOffset, _robot.location.getY() + yOffset);

	set<Point> newNLoc;
	for (auto it = _NLocations.begin(); it != _NLocations.end(); ++it)
	{
		newNLoc.insert(Point(it->getX() + xOffset, it->getY() + yOffset));
	}
	_NLocations = newNLoc;

	set<Point> newDirtyLoc;
	for (auto it = _dirtyLocations.begin(); it != _dirtyLocations.end(); ++it)
	{
		newDirtyLoc.insert(Point(it->getX() + xOffset, it->getY() + yOffset));
	}
	_dirtyLocations = newDirtyLoc;

	_docking = Point(_docking.getX() + xOffset, _docking.getY() + yOffset);
}

void AlgorithmBase::expandMatrix()
{
	if (_robot.location.getY() == _houseHeight -2)
	{
		unsigned oldHeight = _houseHeight;

		_houseHeight *= 2;
		char** newHouse = new char*[_houseHeight];

		for (size_t i = 0; i < oldHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			strncpy(newHouse[i], _house[i], _houseLength);
			newHouse[i][_houseLength] = '\0';
			delete _house[i];
		}

		for (size_t i = oldHeight; i < _houseHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			newHouse[i][_houseLength] = '\0';
			memset(newHouse[i], AlgorithmBase::UNKNOWN, _houseLength);
		}

		delete _house;
		_house = newHouse;
	}


	if (_robot.location.getX() == _houseLength - 2)
	{
		unsigned oldLength = _houseLength;

		_houseLength *= 2;
		char** newHouse = new char*[_houseLength];

		for (size_t i = 0; i < _houseHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			strncpy(newHouse[i], _house[i], oldLength);
			memset(newHouse[i] + oldLength, AlgorithmBase::UNKNOWN, oldLength);

			newHouse[i][_houseLength] = '\0';
			delete _house[i];
		}

		delete _house;
		_house = newHouse;
	}

	if (_robot.location.getY() == 1)
	{
		unsigned int oldHeight = _houseHeight;

		_houseHeight *= 2;
		char** newHouse = new char*[_houseHeight];

		for (size_t i = 0; i < oldHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			newHouse[i][_houseLength] = '\0';
			memset(newHouse[i], AlgorithmBase::UNKNOWN, _houseLength);
		}

		for (size_t i = oldHeight; i < _houseHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			strncpy(newHouse[i], _house[i-oldHeight], _houseLength);
			newHouse[i][_houseLength] = '\0';
			delete _house[i - oldHeight];
		}

		delete _house;
		_house = newHouse;

		updatePoints(0, oldHeight);
	}

	if (_robot.location.getX() == 1)
	{
		unsigned oldLength = _houseLength;

		_houseLength *= 2;
		char** newHouse = new char*[_houseLength];

		for (size_t i = 0; i < _houseHeight; i++)
		{
			newHouse[i] = new char[_houseLength + 1];
			memset(newHouse[i] , AlgorithmBase::UNKNOWN, oldLength);
			strncpy(newHouse[i] + oldLength, _house[i], oldLength);

			newHouse[i][_houseLength] = '\0';
			delete _house[i];
		}

		delete _house;
		_house = newHouse;

		updatePoints(oldLength, 0);
	}
}

void AlgorithmBase::updateAfterMove(Direction direction_)
{
	// update robot info
	_robot.location.move(direction_);
	_robot.totalSteps++;

	expandMatrix();

	dijakstra(_docking, _dijakstraHome);
	updateRemainingMoves();

	updateBattery();
	
//	// Save moves
//	if (!_aboutToFinishCalled)
//	{
//		_lastMove = direction_;
//
//		// No need to store stay moves
//		if (_lastMove != Direction::Stay)
//		{
//			_movesDone.push_back(_lastMove);
//		}
//	}
}

Direction AlgorithmBase::recoverFromUndisciplinedRobot(Direction actualPrevStep_)
{
	if (_mode == DIJAKSTRA)
	{
		_dijakstraToDest.push_back(_lastMove);
	}
	else if (_mode == RETURNHOME || _mode == LOWBATTERY)
	{
		_dijakstraHome.push_back(_lastMove);
	}
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
	return "NULL";
}

void AlgorithmBase::updateHouseKnowladge(SensorInformation info)
{
	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		Direction direction = (Direction) i;
		Point block = _robot.location;

//#ifdef _DEBUG_
//		cout << "Block before move:" << block << endl;
//		cout << "move direction:" << DirectionToString(direction) << endl;
//#endif
		block.move(direction);

//#ifdef _DEBUG_
//		cout << "Block after move:" << block << endl;
//#endif

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
	if (dL > 1)
	{
		_dirtyLocations.insert(_robot.location);
	}

	_NLocations.erase(_robot.location);
	_house[_robot.location.getY()][_robot.location.getX()] = (dL == 0) ? EMPTY : CLEAN + dL;
}

Point AlgorithmBase::findClosestPoint(const set<Point>& points)
{
	Point result;
	double minDistance = _houseLength + _houseHeight;
	for (auto it = points.begin(); it != points.end(); ++it)
	{
		double dis = _robot.location.distance(*it);
		if (_robot.location.distance(*it) < minDistance)
		{
			result = *it;
			minDistance = dis;
		}
	}
	return result;
}

int AlgorithmBase::GetMovesToPoint(Point point)
{
	vector<Direction> directions;
	dijakstra(point, directions);

	return directions.size();
}

Direction AlgorithmBase::getMoveScanMode(SensorInformation info, vector<Direction>& possiblemoves)
{
	if (info.dirtLevel > 0)
	{
		return Direction::Stay;
	}
	else
	{
		_dirtyLocations.erase(_robot.location);
	}

	int findN = 0;
	Direction result = Direction::Stay;

	for (auto it = possiblemoves.begin(); it != possiblemoves.end(); it++)
	{
		// looking for new block
		Point block = _robot.location;
		block.move(*it);
		if (_NLocations.find(block) != _NLocations.end())
		{
			if (findN == 0)
			{
				result = *it;
			}
			findN++;
		}
	}

	if (findN > 0)
	{
		return result;
	}

	//	else go To Some N or go home

	if (_NLocations.size() == 0 && _dirtyLocations.size() == 0)
	{
		_mode = RETURNHOME;
		return getMoveReturnHomeMode(possiblemoves);
	}
	else
	{
		_mode = DIJAKSTRA;

		if (_NLocations.size() == 0)
		{
			dijakstra(findClosestPoint(_dirtyLocations), _dijakstraToDest);
			return getMoveDijakstraMode(possiblemoves);
		}

		if (_dirtyLocations.size() == 0)
		{
			dijakstra(findClosestPoint(_NLocations), _dijakstraToDest);
			return getMoveDijakstraMode(possiblemoves);
		}

		Point NDest = findClosestPoint(_NLocations);
		Point dirtyDest = findClosestPoint(_dirtyLocations);
		int movesToNDest = GetMovesToPoint(NDest);
		int movesTodirtyDest = GetMovesToPoint(dirtyDest);
		int dirtyLevel = _house[dirtyDest.getY()][dirtyDest.getX()] - DUST1 - 1;

		Point dest = dirtyDest;
		if (movesTodirtyDest > movesToNDest + 2 * dirtyLevel)
		{
			dest = NDest;
		}

		dijakstra(dest, _dijakstraToDest);
		return getMoveDijakstraMode(possiblemoves);
	}
}

Direction AlgorithmBase::getMoveDijakstraMode(vector<Direction>& vector)
{
	if (_dijakstraToDest.size() == 1)
	{
		_mode = SCAN;
	}

	Direction dir = _dijakstraToDest.back();
	_dijakstraToDest.pop_back();

	return dir;
}

Direction AlgorithmBase::getMoveReturnHomeMode(vector<Direction>& vector)
{
	if (_robot.location == _docking)
	{
		return Direction::Stay;
	}

	Direction dir = _dijakstraHome.back();
	_dijakstraHome.pop_back();

	return dir;
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

	// TODO: battery Check /////////////
	if (_mode == LOWBATTERY)
	{
		return getMoveReturnHomeMode(possibleMoves);
	}

	if (_mode == SCAN)
	{
		return getMoveScanMode(info, possibleMoves);
	}

	if (_mode == DIJAKSTRA)
	{
		return getMoveDijakstraMode(possibleMoves);
	}

	if (_mode == RETURNHOME)
	{
		return getMoveReturnHomeMode(possibleMoves);
	}

	return possibleMoves[deleteThis % possibleMoves.size()];
}

void AlgorithmBase::printHouse(Point robotLocation) const
{
	cout << "####################################################" << endl;
	cout << "Guessed House : " << endl;
	char prevR = _house[robotLocation.getY()][robotLocation.getX()];
	char prevD = _house[_docking.getY()][_docking.getX()];
	_house[robotLocation.getY()][robotLocation.getX()] = 'R';
	_house[_docking.getY()][_docking.getX()] = 'D';
	for (size_t i = 0; i < _houseHeight; i++)
	{
		cout << _house[i] << endl;
	}
	_house[robotLocation.getY()][robotLocation.getX()] = prevR;
	_house[_docking.getY()][_docking.getX()] = prevD;
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
	return _dijakstraHome.size();
}

void AlgorithmBase::updateBattery()
{
	int consumptionRate = _config["BatteryConsumptionRate"];
	unsigned int rechargeRate = _config["BatteryRechargeRate"];
	unsigned int capacity = _config["BatteryCapacity"];

	if (isDocking())
	{
		_robot.battery = std::min(_robot.battery + rechargeRate, capacity);
	}
	else
	{
		_robot.battery -= consumptionRate;
	}

	int movesToDocking = NumberOfMovesToDocking();
	int returnBatteryConsumption = movesToDocking * consumptionRate;

	// Next were figuring out if we have enough battery to return home
	// The 3 marked here is a safety margin //////////////////////////   -|-  /////////////////////
	if ((_robot.battery >= returnBatteryConsumption) && (_robot.battery - 3 * consumptionRate <= returnBatteryConsumption))
	{
		_mode = LOWBATTERY;
		return;
	}

	if (_robot.battery == capacity)
	{
		_mode = SCAN;
	}

	// The 3 marked here is a safety margin /////-|-/////////////////////
	if (_aboutToFinishCalled && (movesToDocking + 3 > _movesUntilFinish) && movesToDocking < _movesUntilFinish)
	{
		_mode = RETURNHOME;
	}
}
