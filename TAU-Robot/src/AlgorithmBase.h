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

#define MAXHOUSELENGTH 96

class AlgorithmBase : public AbstractAlgorithm
{

public:
	AlgorithmBase();
	void freeHouse(int height);
	~AlgorithmBase();
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
	int _movesUntilFinish = 100000000;

	//////////// For Algo /////////

	enum { DOCKING = 'D', WALL = 'W', CLEAN = '0', EMPTY = ' ', NOTWALL = 'N', UNKNOWN = '?', ROBOT = 'R' };
	enum { DUST1 = '1', DUST2, DUST3, DUST4, DUST5, DUST6, DUST7, DUST8, DUST9 };
	enum Mode { RETURNHOME, SCAN, DIJAKSTRA, LOWBATTERY};

	char** _house;

	int _houseLength = MAXHOUSELENGTH;
	int _houseHeight = MAXHOUSELENGTH;
	Mode _mode = Mode::SCAN;
	Mode _prevMode = Mode::SCAN;
	set<Point> _NLocations;
	set<Point> _dirtyLocations;
	Point		_docking;
	int _undisciplinedCount = 0;

	// List of dirs to destination.. stored in reverse for convinience
	vector<Direction> _dijakstraToDest;

	// List of dirs to destination.. stored in reverse for convinience
	vector<Direction> _dijakstraHome;


	bool isDocking() const;
	static Direction oppositeDirection(Direction direction_);
	void updateRemainingMoves();
	size_t NumberOfMovesToDocking();
	double getUndisciplinedRate();
	void updateBeforeMove(Direction direction_);
	void updatePointsSet(set<Point>& points, unsigned xOffset, unsigned yOffset);
	void updatePoints(unsigned xOffset, unsigned yOffset);
	void expandMatrix();
	void updateAfterMove(Direction direction_);
	Direction recoverFromUndisciplinedRobot(Direction prevMove_, SensorInformation info, vector<Direction>& possiblemoves);
	string DirectionToString(Direction direction) const;
	void updateHouseKnowladge(SensorInformation info);
	Point findClosestPoint(const set<Point>& points);
	int GetMovesToPoint(Point point);
	Direction getMoveScanMode(SensorInformation info, vector<Direction>& possiblemoves);
	size_t movesUntilNoBattery();
	Direction getMoveDijakstraMode(vector<Direction>& possiblemoves);
	Direction getMoveReturnHomeMode(vector<Direction>& vector);
	Direction getMove(Direction prevMove_, vector<Direction>& order);
	void printHouse(Point robotLocation) const;
	void printNLocation();
	void dijakstra(Point dest_, vector<Direction>& result);
	int calcScoreForPath(int get, Point point);
	void dijakstraHome(Point dest_, vector<Direction>& result_);

};


#endif //__ALGORITHM_BASE__H_