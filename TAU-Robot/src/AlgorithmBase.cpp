#include "AlgorithmBase.h"

void AlgorithmBase::aboutToFinish(int stepsTillFinishing)
{
	_returnHome = true;
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

		// If it will take too long to return the same way, we'll have to improvise..
		// No need for a flag here, as 
		if ((int)_movesDone.size() > abs(_eastLocation) + abs(_southLocation))
		{
//			returnQuick = true;
		}
		else
		{
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
	}

	SensorInformation info = _sensor->sense(); // info.isWall = { East, West, South, North }

	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		if (!info.isWall[i])
		{
			moves_.push_back((Direction)i);
		}
	}

	// skipping return quick (difficult and buggy as hell), letting the robot do his thing, maybe we'll get lucky
//	if (returnQuick)
//	{
//		if (returnQuicklyMoves(moves_))
//		{
//			return;
//		}
//	}

	// Bigger than one because we'll go back the same way
	if (info.dirtLevel > 1 || moves_.size() == 0 || _robot.location == Point())
	{
		// add Stay option only if needed
		// For the algorithm, the docking station is at the point (0,0)
		moves_.push_back(Direction::Stay);
	}

	// No need to go back
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
	return _eastLocation == 0 && _southLocation == 0;
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

void AlgorithmBase::updateLocation(Direction direction_)
{
	if (direction_ == Direction::East){
		_eastLocation++;
	}
	else if (direction_ == Direction::West){
		_eastLocation--;
	}
	else if (direction_ == Direction::South){
		_southLocation++;
	}
	else if (direction_ == Direction::North){
		_southLocation--;
	}
}

