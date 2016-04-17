#include "Algorithm_A.h"

Direction Algorithm_A::step()
{
	updateBeforeMove();
	
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	// Choose "randomly"
	Direction next = moves[_psuedoRand % moves.size()];

	updatePseudoRandom();
	updateAfterMove(next);

	return next;
}

void Algorithm_A::updatePseudoRandom()
{	
	_psuedoRand++;

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand += 3;
	}
}

extern "C" AbstractAlgorithm* getAbstractAlgorithmPointer()
{
//	cout << "in getAbstractFunctionPointer" << endl << std::flush;

	return new Algorithm_A();
}
