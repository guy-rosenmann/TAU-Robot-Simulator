#include "Algorithm.h"

Direction Algorithm::step()
{
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	return moves[std::rand() % moves.size()];
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

	if (info.dirtLevel > 0 || moves_.size() == 0)
	{
		// add Stay option only if needed
		moves_.push_back(Direction::Stay);
	}
}