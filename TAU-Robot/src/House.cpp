#include "House.h"

#include <algorithm>
#include <cstring>

House::House()
{
	enum { rows = 19, cols = 80 }; // this is an example for an hard coded house...

	_rows = rows;
	_cols = cols;
	_name = "Example House";
	_description = "Example house from recitation";
	
	char house[rows][cols + 1] = {
	//             1         2         3         4         5         6         7        
	//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		"W  99   D              1234321                                                 W", // 1
		"W  99      WWWWWWW     1234321                     W                       1   W", // 2
		"W              W                                   W   555                 2   W", // 3
		"W              W                                   W   555                 3   W", // 4
		"W              W           WWWWWWWWWWWWWWWWWWWWWWWWW                       4   W", // 5
		"W              W                                                           5   W", // 6
		"W              W                                                           6   W", // 7
		"W                          WWWWWWWWWWWWWWWWWWWWWW  WWWWWWW                 7   W", // 8
		"W         1         2         3         4         5W 999 W  6         7        W", // 9
		"W              W           444                     W 999 W                 9   W", // 10
		"W              W           444                     W 999 W                 8   W", // 11
		"W              W                                   W     W                 7   W", // 12
		"W              W                                   WW   WW                 6   W", // 13
		"W              W                                    W   W                  5   W", // 14
		"W              W                                                           4   W", // 15
		"W              W                                                           3   W", // 16
		"W              W                                                               W", // 17
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"	// 18
	};

	_house = new char*[_rows];
	for (size_t i = 0; i < _rows; i++)
	{
		_house[i] = new char[_cols + 1];
		std::memcpy(_house[i], house[i], _cols + 1);
	}

	_docking = this->findDocking();
	this->updateDirtCount();
	_totalDirt = _currentDirt;
}


House::House(const House& other)
{
	setHouse(other);
}


House::~House()
{
	freeHouse();
}


House& House::operator=(const House& other)
{
	if (_house != other._house) { // prevent self destruction in case of self assignment
		setHouse(other);
	}
	return *this;
}

const char House::operator[](const Point& p) const
{
	if (!this->isInside(p))
	{
		return House::ERR;
	}

	return _house[p.getY()][p.getX()];
}

char& House::operator[](const Point& p)
{
	if (!this->isInside(p))
	{
		throw House::ERR;
	}

	return _house[p.getY()][p.getX()];
}

void House::print(ostream& out) const
{
	out << endl;
	out << "          1         2         3         4         5         6         7         " << endl;
	out << "01234567890123456789012345678901234567890123456789012345678901234567890123456789" << endl;
	for (size_t row = 0; row < _rows; ++row)
	{
		out << _house[row] << " " << row << endl;
	}
	out << endl << "docking station at: " << _docking << endl;
}

void House::freeHouse()
{
	if (_house != nullptr)
	{
		for (size_t i = 0; i < _rows; i++)
		{
			delete[] _house[i];
		}

		delete[] _house;
		_house = nullptr;
	}
}

void House::setHouse(const House& other)
{
	freeHouse();

	_rows = other._rows;
	_cols = other._cols;
	_docking = other._docking;
	_totalDirt = other._totalDirt;
	_currentDirt = other._currentDirt;

	_house = new char*[_rows];
	for (size_t i = 0; i < _rows; i++)
	{
		_house[i] = new char[_cols + 1];
		std::memcpy(_house[i], other._house[i], _cols + 1);
	}

	this->updateDirtCount();
}


Point House::findDocking()
{
	for (size_t row = 0; row < _rows; ++row)
	{
		for (size_t col = 0; col < _cols; ++col)
		{
			if (_house[row][col] == House::DOCKING) {
				return Point(col, row); // col == X, row == Y
			}
		}
	}
	
	throw House::ERR; // couldn't find docking
}


int House::clean(Point& spot_, int amout_)
{
	if (!this->isInside(spot_) || (*this)[spot_] == House::WALL)
	{
		return 0;
	}

	if ((*this)[spot_] >= House::DUST1 && (*this)[spot_] <= House::DUST9)
	{
		int dirt = (*this)[spot_] - House::CLEAN;
		int cleaned = (char)std::min(dirt, amout_);

		(*this)[spot_] -= cleaned;
		if ((*this)[spot_] <= House::CLEAN)
		{
			(*this)[spot_] = House::EMPTY;
		}
		
		_currentDirt -= cleaned;
		return cleaned;
	}

	return 0;
}

void House::updateDirtCount()
{
	_currentDirt = 0;
	for (size_t i = 0; i < _rows; ++i)
	{
		for (size_t j = 0; j < _cols; ++j)
		{
			switch (_house[i][j])
			{
			case House::CLEAN:
				_house[i][j] = House::EMPTY; // convert '0' dirt chars to space chars
				break;
			case House::DUST1:
			case House::DUST2:
			case House::DUST3:
			case House::DUST4:
			case House::DUST5:
			case House::DUST6:
			case House::DUST7:
			case House::DUST8:
			case House::DUST9:
				_currentDirt += _house[i][j] - House::CLEAN;
				break;
			default:
				break;
			}
		}
	}
}