#include "Algorithm_B.h"

Direction Algorithm_B::step()
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

void Algorithm_B::updatePseudoRandom()
{
	_psuedoRand++;

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand++;
	}
}

extern "C" AbstractAlgorithm* getAbstractAlgorithmPointer()
{
	//	cout << "in getAbstractFunctionPointer" << endl << std::flush;

	return new Algorithm_B();
}