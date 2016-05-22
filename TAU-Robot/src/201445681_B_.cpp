#include "201445681_B_.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"
#include <boost/filesystem/path.hpp>

_201445681_B::_201445681_B()
{
	_myOrder.push_back(Direction::East);
	_myOrder.push_back(Direction::South);
	_myOrder.push_back(Direction::North);
	_myOrder.push_back(Direction::West);
}

Direction _201445681_B::step(Direction prevStep_)
{
	// AfterMove because we can't be sure it moved (in cases of undisciplined robot)
	updateAfterMove(prevStep_);

	Direction next = getMove(prevStep_, _myOrder);

	// BeforeMove because we can't be sure it moved (in cases of undisciplined robot)
	updateBeforeMove(next);

#ifdef _DEBUG_

//	if (deleteThis++ % 10 == 0)
//		{
//	if (_robot.totalSteps > 55)
//	{
//
//		boost::detail::Sleep(50);
//		system("cls");
//		printHouse(_robot.location);
//		cout << "B" << endl;
//		cout << _robot.location << endl;
//		cout << (int)prevStep_ << endl;
//	}
//		}

#endif

	return next;
}
#ifndef _WINDOWS_
REGISTER_ALGORITHM(_201445681_B)
#endif