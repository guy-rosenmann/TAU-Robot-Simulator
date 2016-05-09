#include "201445681_A_.h"
#include "AbstractAlgorithm.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"

#include <boost/filesystem/path.hpp>

Direction _201445681_A::step(Direction prevStep_)
{

	// AfterMove because we can't be sure it moved (in cases of undisciplined robot)
	updateAfterMove(prevStep_);

	Direction next = getMove(prevStep_);

	// BeforeMove because we can't be sure it moved (in cases of undisciplined robot)
	updateBeforeMove(next);

#ifdef _DEBUG_
	//	int i = 0;
	boost::detail::Sleep(50);
	system("cls");
	printHouse(_robot.location);
	cout << _robot.location << endl;
	cout << (int)prevStep_ << endl;

#endif

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


#ifndef _WINDOWS_
REGISTER_ALGORITHM(_201445681_A)
#endif