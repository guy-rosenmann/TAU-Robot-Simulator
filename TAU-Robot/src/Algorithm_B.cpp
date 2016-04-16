#include "Algorithm_B.h"

Direction Algorithm_B::step()
{
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	Direction next = moves[_psuedoRand++ % moves.size()];

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand++;
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

//extern "C" 
//{
//	AbstractAlgorithm *maker_B()
//	{
//		return new Algorithm_B;
//	}
//
//	class proxy 
//	{
//		public:
//			proxy(){
//				// register the maker with the factory 
//				factory["201445681_B_"] = maker_B;
//			}
//	};
//	// our one instance of the proxy
//	proxy p_B;
//}