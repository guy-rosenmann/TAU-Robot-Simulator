#include "Algorithm.h"

Direction Algorithm::step()
{
	SensorInformation info = _sensor->sense(); // info.isWall = { East, West, South, North }
	
	// check if stuck
	unsigned int j;
	for (j = 0; info.isWall[j] && j < sizeof(info.isWall); ++j);
	if (j == sizeof(info.isWall)/sizeof(bool))
	{
		return Direction::Stay;
	}

	int move = std::rand() % 5; // random move
	if (move == (int)Direction::Stay)
	{
		return (Direction)move;
	}
	
	// find first matched direction (not a wall)
	j = 0;
	for (int i = 0; i <= move; ++i)
	{
		j++;
		while (info.isWall[j % 4]) j++;
	}
	return (Direction)(j % 4);
}