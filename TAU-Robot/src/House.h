#ifndef __HOUSE__H_
#define __HOUSE__H_

#include <iostream>

#include "Point.h"

using namespace std;


class House
{

	size_t	_maxSteps;
	size_t	_rows;
	size_t	_cols;
	char**	_house = nullptr;

	string	_name;
	
	Point	_docking;
	int		_totalDirt;
	int		_currentDirt;

	string _houseFilename;
	bool	_isValid;
	string	_errorLine;


public:
	enum { DOCKING = 'D', WALL = 'W', CLEAN = '0', EMPTY = ' ' };
	enum { DUST1 = '1', DUST2, DUST3, DUST4, DUST5, DUST6, DUST7, DUST8, DUST9 };
	enum { ERR = 0, FILE_ERR };

	House(const char* path_ = NULL);
	House(const House& other) { setHouse(other); }
	House(House&& other);
	virtual ~House() { freeHouse(); }

	House& operator=(const House& other);
	House& operator=(House&& other);
	
	
	int clean(Point& spot_, int amout_ = 1);
	bool isClean() const { return _currentDirt == 0; }
	char at(const Point& p) const { return (*this)[p]; }

	
	// getters
	string getName() const { return _name; }
	size_t getMaxSteps() const { return _maxSteps; }
	size_t getXSize() const { return _cols; }
	size_t getYSize() const { return _rows; }
	int getTotalDirtAmount() const { return _totalDirt; }
	int getDirtAmount() const { return _currentDirt; }
	Point getDocking() const { return _docking; }
	bool isValid() const { return _isValid; }
	string getErrorLine() const { return _errorLine; }
	string getFileName() const { return _houseFilename; }

	// prints
	void print(ostream& out = cout) const;
	void print(const Point& robot, ostream& out = cout) const;
	friend ostream& operator<<(ostream& out, const House& h) { h.print(out); return out; }

private:
	Point findDocking();
	void freeHouse();
	void setHouse(const House& other);

	void createDefaultHouse();
	void loadFromFile(const char* path_);

	const char operator[](const Point& p) const;
	char& operator[](const Point& p);
	bool isInside(const Point& p) const { return (p.getX() < (int)_cols) && (p.getX() >= 0) && (p.getY() < (int)_rows) && (p.getY() >= 0); }
	void validateHouse();
	bool GetUnsignedIntFromStream(ifstream& fin_, size_t* argPointer_, unsigned int rowNumber_);
};



#endif //__HOUSE__H_