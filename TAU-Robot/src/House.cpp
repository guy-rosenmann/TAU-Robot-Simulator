#include "House.h"
#include "BoostUtils.h"

#include <cstring>
#include <cmath>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>


House::House(const char* path_)
{
	boost::filesystem::path path(path_);
	_houseFilename = path.filename().generic_string();
	_houseFilenameWithoutSuffix = path.stem().generic_string();
	this->loadFromFile(path_);

#ifdef _DEBUG_
//	this->print(_docking);
	cout << endl << "Docking station: " << _docking << endl << endl;
#endif
}


void House::loadFromFile(const char* path_)
{
	_isValid = true;
	ifstream fin(path_);

	if (!fin.is_open() || !fin.good())
	{

#ifdef _DEBUG_
		cout << "[ERROR] Failed to load house from: " << path_ << endl;
#endif
		_isValid = false;
		_errorLine = _houseFilename + ": cannot open file";
		return;
	}

	freeHouse();
	
	std::getline(fin, _name);
	if (!GetUnsignedIntFromStream(fin, &_maxSteps, 2) || !GetUnsignedIntFromStream(fin, &_rows, 3) || !GetUnsignedIntFromStream(fin, &_cols, 4))
	{
		fin.close();
		return;
	}
	
	fin.ignore();	//skip newline and go the begining of matrix

	_house = new char*[_rows];
	for (size_t i = 0; i < _rows; i++)
	{
		_house[i] = new char[_cols + 1];
		_house[i][_cols] = '\0';
		memset(_house[i], House::EMPTY, _cols); // fill in all places with spaces

		string houseRow;
		std::getline(fin, houseRow);
		memcpy(_house[i], houseRow.c_str(), std::min(houseRow.size(), _cols));
	}

	fin.close();

	this->validateHouse();
}

bool House::GetUnsignedIntFromStream(ifstream& fin_, size_t* argPointer_, unsigned int rowNumber_)
{
	string line;
	bool validLine = true;
	int num = 0;

	std::getline(fin_, line);
	try
	{
		num = stoi(line);
	}
	catch (std::invalid_argument e)
	{
		validLine = false;
	}
	catch (std::out_of_range e)
	{
		validLine = false;
	}

	if (num <= 0)
	{
		validLine = false;
	}

	if (!validLine)
	{
#ifdef _DEBUG_
		cout << "[ERROR] Invalid line " << rowNumber_ << " " << line << endl;
#endif
		_isValid = false;
		_errorLine = _houseFilename + ": line number " + std::to_string(rowNumber_) + " in house file shall be a positive number, found: " + line;
		return false;
	}

	*argPointer_ = num;
	return true;
}

void House::setHouse(const House& other)
{
	freeHouse();

	_maxSteps = other._maxSteps;
	_rows = other._rows;
	_cols = other._cols;
	_docking = other._docking;
	_totalDirt = other._totalDirt;
	_currentDirt = other._currentDirt;

	_houseFilename = other._houseFilename;
	_houseFilenameWithoutSuffix = other._houseFilenameWithoutSuffix;

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


House::House(House&& other) :	_maxSteps(other._maxSteps),
								_rows(other._rows),
								_cols(other._cols),
								_name(other._name),
								_docking(other._docking),
								_totalDirt(other._totalDirt),
								_currentDirt(other._totalDirt),
								_houseFilename(other._houseFilename),
								_houseFilenameWithoutSuffix(other._houseFilenameWithoutSuffix),
								_isValid(other._isValid),
								_errorLine(other._errorLine)
{
	std::swap(_house, other._house);
}


House& House::operator=(const House& other)
{
	if (_house != other._house) { // prevent self destruction in case of self assignment
		setHouse(other);
	}
	return *this;
}

House& House::operator=(House&& other)
{
	_maxSteps = other._maxSteps;
	_rows = other._rows;
	_cols = other._cols;
	std::swap(_house, other._house);

	_name = other._name;
	_docking = other._docking;
	_totalDirt = other._totalDirt;
	_currentDirt = other._totalDirt;
	_houseFilename = other._houseFilename;
	_houseFilenameWithoutSuffix = other._houseFilenameWithoutSuffix;
	_isValid = other._isValid;
	_errorLine = other._errorLine;

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
		throw (int)House::ERR;
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


void House::print(const Point& robot, ostream& out) const
{
	if (this->isInside(robot))
	{
		char last = (*this)[robot];
		
		// Const hack - temporarily changing character in house matrix, then changing it back (so overall - no change!)
		House& h = const_cast<House&>(*this);
		h[robot] = 'R';
		this->print(out);
		h[robot] = last;
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
	
	throw (int)House::ERR; // couldn't find docking
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
void House::validateHouse()
{
	bool fixedWalls = false;
	int dockingCount = 0;

	_currentDirt = 0; // update current dirt count
	_isValid = true; 
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
#ifdef _DEBUG_
		cout << "[WARN] House did not have a full surronding wall ! Missing walls were added." << endl;
#endif
	}

	if (dockingCount == 0)
	{
#ifdef _DEBUG_
		cout << "[WARN] House does not have a docking station." << endl;
#endif
		_isValid = false;
		_errorLine = _houseFilename + ": missing docking station (no D in house)";
	}
	else if (dockingCount != 1)
	{
#ifdef _DEBUG_
		cout << "[WARN] House has more than 1 docking station." << endl;
#endif
		_isValid = false;
		_errorLine = _houseFilename + ": too many docking stations (more than one D in house)";
	}

	_totalDirt = _currentDirt;
}


vector<string> House::getMontageTiles(const Point& robot_) const
{
	vector<string> tiles;
	for (size_t row = 0; row < _rows; ++row)
	{
		for (size_t col = 0; col < _cols; ++col)
		{
			if (robot_ == Point(col, row))
			{
				tiles.push_back("R");
			}
			else if (_house[row][col] == House::EMPTY || _house[row][col] == House::CLEAN)
			{
				tiles.push_back("0");
			}
			else
			{
				tiles.push_back(string() + _house[row][col]);
			}		
		}
	}
	
	return tiles;
}