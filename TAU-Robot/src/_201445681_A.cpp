#include "_201445681_A.h"

Direction _201445681_A::step()
{
	updateBeforeMove();
	
	vector<Direction> moves;
	this->getPossibleMoves(moves);

	// Choose "randomly"
	Direction next = moves[_psuedoRand % moves.size()];

	updatePseudoRandom();
	updateAfterMove(next);

	// REMOVE
//	cout << "A: moves.size(): " << moves.size() << endl;
	
	return next;
}

void _201445681_A::updatePseudoRandom()
{	
	_psuedoRand++;

	// Adding more randomness
	if (_psuedoRand % 11 == 0)
	{
		_psuedoRand += 3;
	}
}

void _201445681_A::resetValues()
{
	AlgorithmBase::resetValues();
	_psuedoRand = pseudoDefaultValue;
}

extern "C" AbstractAlgorithm* getAbstractAlgorithmPointer()
{
//	cout << "in getAbstractFunctionPointer" << endl << std::flush;

	return new _201445681_A();
}
