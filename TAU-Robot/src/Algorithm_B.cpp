#include "Algorithm_B.h"

Direction Algorithm_B::step()
{
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	Direction next = moves[_psuedoRand % moves.size()];

	// update robot info
	_robot.location.move(next);
	_robot.totalSteps++;

	return next;
}

void Algorithm_B::getPossibleMoves(vector<Direction>& moves_)
{
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
}

extern "C" 
{
	AbstractAlgorithm *maker()
	{
		return new Algorithm_B;
	}

	class proxy 
	{
		public:
			proxy(){
				// register the maker with the factory 
				factory["201445681_B_"] = maker;
			}
	};
	// our one instance of the proxy
	proxy p;
}