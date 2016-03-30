#include "Algorithm.h"

Direction Algorithm::step()
{
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	Direction next = moves[std::rand() % moves.size()];
	
	// update robot info
	_robot.location.move(next);
	_robot.totalSteps++;

	return next;
}


void Algorithm::getPossibleMoves(vector<Direction>& moves_)
{
	SensorInformation info = _sensor->sense(); // info.isWall = { East, West, South, North }

	for (unsigned int i = 0; i < sizeof(info.isWall) / sizeof(bool); ++i)
	{
		if (!info.isWall[i])
		{
			moves_.push_back((Direction)i);
		}
	}

	if (info.dirtLevel > 0 || moves_.size() == 0 || _robot.location == Point())
	{
		// add Stay option only if needed
		// For the algorithm, the docking station is at the point (0,0)
		moves_.push_back(Direction::Stay);
	}
}