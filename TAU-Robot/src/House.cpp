#include "House.h"


#include <string>
#include <cstring>
#include <cmath>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>


const char* House::defaultHouseFileName = "simple1.house";



House::House(const char* path_)
{
	if (path_ == NULL)
	{
		cout << "[INFO] Using default house: " << House::defaultHouseFileName << endl;

		if (!this->loadFromFile(House::defaultHouseFileName))
		{
			cout << "[INFO] No default house - creating one." << endl;
			createDefaultHouse();
			this->loadFromFile(House::defaultHouseFileName);
		}
	}
	else
	{
		if (!this->loadFromFile(path_))
		{
			cout << "[ERROR] Failed to load house from: " << path_ << endl;
			throw House::ERR;
		}
	}

	


	//this->print(_docking);
	//cout << endl << "Docking station: " << _docking << endl << endl;
}


void House::createDefaultHouse()
{
	ofstream fout(House::defaultHouseFileName);

	if (fout.good())
	{
		fout << "Simple1" << endl;

#if 0
		fout << "Really big house" << endl;
		fout << 19 << endl;
		fout << 80 << endl;
		fout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << endl;
		fout << "W  99   D              1234321                                                 W" << endl;
		fout << "W  99      WWWWWWW     1234321                     W                       1   W" << endl;
		fout << "W              W                                   W   555                 2   W" << endl;
		fout << "W              W                                   W   555                 3   W" << endl;
		fout << "W              W           WWWWWWWWWWWWWWWWWWWWWWWWW                       4   W" << endl;
		fout << "W              W                                                           5   W" << endl;
		fout << "W              W                                                           6   W" << endl;
		fout << "W                          WWWWWWWWWWWWWWWWWWWWWW  WWWWWWW                 7   W" << endl;
		fout << "W         1         2         3         4         5W 999 W  6         7        W" << endl;
		fout << "W              W           444                     W 999 W                 9   W" << endl;
		fout << "W              W           444                     W 999 W                 8   W" << endl;
		fout << "W              W                                   W     W                 7   W" << endl;
		fout << "W              W                                   WW   WW                 6   W" << endl;
		fout << "W              W                                    W   W                  5   W" << endl;
		fout << "W              W                                                           4   W" << endl;
		fout << "W              W                                                           3   W" << endl;
		fout << "W              W                                                               W" << endl;
		fout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" << endl;
#else
		fout << "2 Bedrooms + Kitchen Isle" << endl;
		fout << 8 << endl;
		fout << 10 << endl;
		fout << "WWWWWWWWWW" << endl;
		fout << "W22  DW59W" << endl;
		fout << "W  W 1119W" << endl;
		fout << "W WWW3WW W" << endl;
		fout << "W6   3W  W" << endl;
		fout << "W78W  W  W" << endl;
		fout << "W99W  W  W" << endl;
		fout << "WWWWWWWWWW" << endl;
#endif

	}

	fout.close();
}


bool House::loadFromFile(const char* path_)
{
	ifstream fin(path_);

	if (!fin.good())
	{
		fin.close();
		return false;
	}

	freeHouse();
	
	std::getline(fin, _name);
	std::getline(fin, _description);
	fin >> _rows;
	fin >> _cols;
	fin.ignore();	//skip newline and go the begining of matrix

	_house = new char*[_rows];
	for (size_t i = 0; i < _rows; i++)
	{
		_house[i] = new char[_cols + 1];
		_house[i][_cols] = '\0';
		std::memset(_house[i], House::EMPTY, _cols); // fill in all places with spaces

		string houseRow;
		std::getline(fin, houseRow);
		std::memcpy(_house[i], houseRow.c_str(), std::min(houseRow.size(), _cols));
	}

	fin.close();

	if (!this->validateHouse())
	{
		throw House::ERR;
	}

	return true;
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

	this->validateHouse();
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

	string colMajor, colMinor;
	for (size_t i = 0; i < _cols; ++i)
	{
		if (i != 0 && i % 10 == 0)
		{
			colMajor += std::to_string(i / 10);
		}
		else
		{
			colMajor += " ";
		}
		colMinor += std::to_string(i % 10);
	}
	
	if (_cols > 10)
	{
		out << colMajor << endl;
	}
	out << colMinor << endl;

	for (size_t row = 0; row < _rows; ++row)
	{
		out << _house[row] << " " << row << endl;
	}
	
	//out << endl << "Docking station at: " << _docking << endl;
}


void House::print(const Point& robot, ostream& out)
{
	if (this->isInside(robot))
	{
		char last = (*this)[robot];
		(*this)[robot] = 'R';
		this->print(out);
		(*this)[robot] = last;
	}
	else
	{
		this->print(out);
	}
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
	House& house = *this;
	
	if (!this->isInside(spot_))
	{
		return 0;
	}

	if (house[spot_] >= House::DUST1 && house[spot_] <= House::DUST9)
	{
		int dirt = house[spot_] - House::CLEAN;
		int cleaned = (char)std::min(dirt, amout_);

		house[spot_] -= cleaned;
		if (house[spot_] <= House::CLEAN)
		{
			house[spot_] = House::EMPTY;
		}
		
		_currentDirt -= cleaned;
		return cleaned;
	}

	return 0;
}


// Update dirt count
// Add surronding wall if missing
bool House::validateHouse()
{
	bool fixedWalls = false;
	int dockingCount = 0;

	_currentDirt = 0; // update current dirt count

	for (size_t i = 0; i < _rows; ++i)
	{
		for (size_t j = 0; j < _cols; ++j)
		{
			char& curr = _house[i][j];

			if (i == 0 || i == _rows - 1 || j == 0 || j == _cols - 1)
			{
				// fix surronding wall if needed
				if (curr != House::WALL)
				{
					curr = House::WALL;
					fixedWalls = true;
				}
			}
			else if (curr == House::CLEAN)
			{
				curr = House::EMPTY; // convert '0' dirt chars into ' ' chars
			}
			else if (curr >= House::DUST1 && curr <= House::DUST9)
			{
				_currentDirt += curr - House::CLEAN; // update current dirt count
			}
			else if (curr == House::DOCKING)
			{
				dockingCount++;
				_docking = Point(j,i);
			}
		}
	}

	if (fixedWalls)
	{
		cout << "[WARN] House did not have a full surronding wall ! Missing walls were added." << endl;
	}

	if (dockingCount != 1)
	{
		cout << "[WARN] House does not have exactly 1 docking station. Skipping the house..." << endl;
		return false;
	}

	_totalDirt = _currentDirt;

	return true;
}