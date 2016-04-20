#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <vector>
#include <string>

using namespace std;

#include "Simulation.h"
#include "AlgorithmLoader.h"
#include "AlgorithmRegistrar.h"

#define ALGO_NAME_CELL_SIZE 13
#define CELL_SIZE 10

class Simulator
{
	Configuration				_config;
	vector<House*>				_houses;
	bool						_successful = false;
	vector<string>				_errors;

	typedef std::pair<AlgorithmLoader*, std::vector<int>*> AlgoPair;
	typedef std::vector< AlgoPair > AlgoVector;
	AlgoVector					_algos;

public:
	Simulator(const Configuration& conf_, const char* housePath_ = NULL, const char* algorithmPath_ = NULL);
	~Simulator();

	bool isReady() { return _successful; }
	void simulate();

private:
	void score(int simulationSteps, vector<Simulation*>& simulatios_);
	int getActualPosition(vector<Simulation*>& allSimulatios_, Simulation& currSimulation_) const;
	void printScores() const;
	void printErrors() const;

	static int CountSpaces(double avg);

	vector<House*> loadAllHouses(const char* house_path);
	vector<AlgorithmLoader*> loadAllAlgos(const char* algorithm_path);
	vector<string> loadFilesWithSuffix(const char* path, const char* suffix);

	bool getAlgos(const char* algorithmPath_, vector<string>& errors_);
	bool getHouses(const char* housePath_);

	template <class T>
	static void clearPointersVector(vector<T*>& vec);
};


#endif //__SIMULATOR__H_