#include "201445681_C_.h"
#include "AlgorithmRegistration.h"


Direction _201445681_C::step(Direction prevStep)
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

#ifndef _WINDOWS_
REGISTER_ALGORITHM(_201445681_C)
#endif