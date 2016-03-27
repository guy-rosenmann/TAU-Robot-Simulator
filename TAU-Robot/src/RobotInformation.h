#ifndef __ROBOT_INFORMATION__H_
#define __ROBOT_INFORMATION__H_

#include "Point.h"

struct RobotInformation
{
	Point location;	// current robot's location
	int battery;	// current battery capacity

	bool goodBehavior;	// didn't try to make an impossible move
	bool stuck;			// ran out of battery (and not in docking station)

	int totalSteps;		// total number of steps
	int cleanedDirt;	// total dirt cleaned

	RobotInformation() : goodBehavior(true), stuck(false), totalSteps(0), cleanedDirt(0) {}
};

#endif //__ROBOT_INFORMATION__H_