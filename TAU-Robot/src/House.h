#ifndef __HOUSE__H_
#define __HOUSE__H_

#include <iostream>

#include "Point.h"

using namespace std;


class House {

	static string defaultHouseFileName;

	size_t	_rows;
	size_t	_cols;
	char**	_house = nullptr;

	string	_name;
	string	_description;
	
	Point	_docking;
	int		_totalDirt;
	int		_currentDirt;


public:
	enum { DOCKING = 'D', WALL = 'W', CLEAN = '0', EMPTY = ' ', ERR = 0 };
	enum { DUST1 = '1', DUST2, DUST3, DUST4, DUST5, DUST6, DUST7, DUST8, DUST9 };

	House();
	House(string& path_);
	House(const House& other) { setHouse(other); }
	virtual ~House() { freeHouse(); }

	House& operator=(const House& other);
	
	char at(const Point& p) const { return (*this)[p]; }
	Point getDocking() const { return _docking; }
	int getTotalDirtAmount() const { return _totalDirt; }
	int getDirtAmount() const { return _currentDirt; }
	bool isClean() const { return _currentDirt == 0; }
	int clean(Point& spot_, int amout_ = 1);

	string getName() const { return _name; }
	string getDescription() const { return _description; }


	void print(ostream& out = cout) const;
	void print(const Point& robot, ostream& out = cout);
	friend ostream& operator<<(ostream& out, const House& h) { h.print(out); return out; }

private:
	Point findDocking();
	void freeHouse();
	void setHouse(const House& other);

	void createDefaultHouse();
	bool loadFromFile(string& path_);

	const char operator[](const Point& p) const;
	char& operator[](const Point& p);
	bool isInside(const Point& p) const { return (p.getX() < (int)_cols) && (p.getX() >= 0) && (p.getY() < (int)_rows) && (p.getY() >= 0); }
	bool validateHouse();
};



#endif //__HOUSE__H_