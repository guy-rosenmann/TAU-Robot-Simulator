#ifndef __POINT__H_
#define __POINT__H_


#include <cmath>
#include <iostream>
#include <functional>

#include "Direction.h"

using namespace std;


class Point {
	int _x, _y;

public:
	Point(int x_, int y_) : _x(x_), _y(y_) {}
	Point() : _x(0), _y(0) {}
	Point(const Point &obj) : _x(obj._x), _y(obj._y) {}
	double distance(const Point& other) const {
		return sqrt(pow(_x - other._x, 2) + pow(_y - other._y, 2));
	}
	int getX()const { return _x; }
	int getY()const { return _y; }

	void print(ostream& out = cout) const { out << "(" << _x << "," << _y << ")"; }
	void move(Direction d) {
		const static function<void(Point&)> do_move[] = {
			[](Point&p){++p._x; },	// East
			[](Point&p){--p._x; },	// West
			[](Point&p){++p._y; },	// South
			[](Point&p){--p._y; },	// North
			[](Point&p){}			// Stay
		};
		do_move[(int)d](*this);
	}
	bool operator==(const Point& other) const { return (_x == other._x && _y == other._y); }
	bool operator<(const Point& other) const { return (_x < other._x) || (_x == other._x && _y < other._y); }
	friend ostream& operator<<(ostream& out, const Point& p) { p.print(out); return out; }
};


#endif //__POINT__H_