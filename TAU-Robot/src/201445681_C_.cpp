#include "201445681_C_.h"

Direction _201445681_C::step()
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

void _201445681_C::updatePseudoRandom()
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
	return new _201445681_C();
}