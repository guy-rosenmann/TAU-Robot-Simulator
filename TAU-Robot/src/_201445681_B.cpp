#include "_201445681_B.h"

Direction _201445681_B::step()
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

void _201445681_B::resetValues()
{
	AlgorithmBase::resetValues();
	_psuedoRand = pseudoDefaultValue;
}

void _201445681_B::updatePseudoRandom()
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

	return new _201445681_B();
}