#include "Algorithm_C.h"

Direction Algorithm_C::step()
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

void Algorithm_C::updatePseudoRandom()
{
	_psuedoRand++;

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand += 2;
	}
}

extern "C" AbstractAlgorithm* getAbstractAlgorithmPointer()
{
	//	cout << "in getAbstractFunctionPointer" << endl << std::flush;

	return new Algorithm_C();
}