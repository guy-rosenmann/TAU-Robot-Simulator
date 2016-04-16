#include "Algorithm_A.h"

Direction Algorithm_A::step()
{
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	Direction next = moves[_psuedoRand++ % moves.size()];

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand+=3;
	}

	// update robot info
	_robot.location.move(next);
	_robot.totalSteps++;
	updateLocation(next);

	// Save moves
	if (!_returnHome)
	{
		_lastMove = next;
		_movesDone.push_back(_lastMove);
	}

	return next;
}

extern "C" 
{
	AbstractAlgorithm *maker_A()
	{
		return new Algorithm_A;
	}

	class proxy 
	{
		public:
			proxy(){
				// register the maker with the factory 
				factory["201445681_A_"] = maker_A;
			}
	};
	// our one instance of the proxy
	proxy p_A;
}