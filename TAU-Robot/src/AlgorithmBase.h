#ifndef __ALGORITHM_BASE__H_
#define __ALGORITHM_BASE__H_

#include <vector>
#include <map>
#include <limits.h>
#include <set>
using namespace std;


#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Configuration.h"
#include "RobotInformation.h"

#define MAXHOUSELENGTH 20
#define INITIALLENGTH (MAXHOUSELENGTH/2)

class AlgorithmBase : public AbstractAlgorithm
{

public:
	AlgorithmBase();
	AlgorithmBase(const AbstractSensor& sensor, const Configuration& conf) { setSensor(sensor); setConfiguration(conf.getParams()); }

	void setSensor(const AbstractSensor& sensor) { _sensor = &sensor; }
	void setConfiguration(map<string, int> config) { _config = config; _robot.battery = _config["BatteryCapacity"]; }

	virtual Direction step(Direction prevStep) = 0;
	void aboutToFinish(int stepsTillFinishing);
	void updateBattery();

protected:
	const AbstractSensor*	_sensor = nullptr;
	map<string, int>		_config;
	RobotInformation		_robot;

	Direction _lastMove = Direction::Stay;
	bool _aboutToFinishCalled = false;
	vector<Direction> _movesDone;
	int _movesUntilFinish = 10000000;

	//////////// For Algo /////////

	enum { DOCKING = 'D', WALL = 'W', CLEAN = '0', EMPTY = ' ', NOTWALL = 'N', UNKNOWN = '?', ROBOT = 'R' };
	enum { DUST1 = '1', DUST2, DUST3, DUST4, DUST5, DUST6, DUST7, DUST8, DUST9 };
	enum Mode { START = 0, RETURNHOME, UNDISCIPLINED, SCAN, DIJAKSTRA, LOWBATTERY};
//	enum ScanMode {FINDSTART, UP, DOWN, LEFT, RIGHT};dijakstra

	char** _house;

	unsigned int _houseLength = MAXHOUSELENGTH;
	unsigned int _houseHeight = MAXHOUSELENGTH;
	Mode _mode = Mode::SCAN;
	Mode _prevMode = Mode::SCAN;
	set<Point> _NLocations;
	set<Point> _dirtyLocations;
	Point		_docking;

	// All steps made since entering a maze
	vector<vector<tuple<Point, Direction>>> _mazeSteps;

	// Quick way home
//	vector<tuple<Point, Direction>> _quickToHome;

	// List of dirs to destination.. stored in reverse for convinience
	vector<Direction> _dijakstraToDest;

	// List of dirs to destination.. stored in reverse for convinience
	vector<Direction> _dijakstraHome;

	int deleteThis = 59;
	///////////////////////////////
	
	bool isDocking() const;
	void getPossibleMoves(vector<Direction>& moves_);
	static Direction oppositeDirection(Direction direction_);
	void removeBackwardDirection(vector<Direction>& moves_) const;
	void updateRemainingMoves();
	size_t NumberOfMovesToDocking() const;
	void updateBeforeMove(Direction direction_);
	void updatePoints(unsigned xOffset, unsigned yOffset);
	void expandMatrix();
	void updateAfterMove(Direction direction_);
	Direction recoverFromUndisciplinedRobot(Direction prevMove_);
	string DirectionToString(Direction direction) const;
	void updateHouseKnowladge(SensorInformation info);
	Point findClosestPoint(const set<Point>& points);
	int GetMovesToPoint(Point point);
	Direction getMoveScanMode(SensorInformation info, vector<Direction>& possiblemoves);
	Direction getMoveDijakstraMode(vector<Direction>& possiblemoves);
	Direction getMoveReturnHomeMode(vector<Direction>& vector);
	Direction getMove(Direction prevMove_);
	Direction goToPoint(Point destination);
	void printHouse(Point robotLocation) const;
	void printNLocation();
	void dijakstra(Point dest_, vector<Direction>& result);
};


#endif //__ALGORITHM_BASE__H_