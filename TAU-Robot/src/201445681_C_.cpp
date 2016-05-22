#include "201445681_C_.h"
#include "MakeUnique.h"
#include "AlgorithmRegistration.h"
#include <boost/filesystem/path.hpp>

_201445681_C::_201445681_C()
{
	_myOrder.push_back(Direction::West);
	_myOrder.push_back(Direction::East);
	_myOrder.push_back(Direction::North);
	_myOrder.push_back(Direction::South);

}

Direction _201445681_C::step(Direction prevStep_)
{
	// AfterMove because we can't be sure it moved (in cases of undisciplined robot)
	updateAfterMove(prevStep_);

	Direction next = getMove(prevStep_, _myOrder);

	// BeforeMove because we can't be sure it moved (in cases of undisciplined robot)
	updateBeforeMove(next);

#ifdef _DEBUG_

//	if (deleteThis % 5 == 0)
//	{
//		boost::detail::Sleep(50);
//		system("cls");
//		printHouse(_robot.location);
//		cout << "C" << endl;
//		cout << _robot.location << endl;
//		cout << (int)prevStep_ << endl;
//	cout << _mode << endl;
	//	}


#endif

	return next;
}


#ifndef _WINDOWS_
REGISTER_ALGORITHM(_201445681_C)
#endif
