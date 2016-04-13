#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <vector>
#include <string>

using namespace std;

#include "Simulation.h"
#include "AlgorithmContainer.h"

#define ALGO_NAME_CELL_SIZE 13
#define CELL_SIZE 13

class Simulator
{


	Configuration				_config;
	vector<House*>				_houses;
	bool						_successful;
	vector<string>				_errors;

	typedef std::pair<AlgorithmContainer*, std::vector<int>*> AlgoPair;
	typedef std::vector< AlgoPair > AlgoVector;
	AlgoVector					_algos;

public:
	Simulator() = delete;
	Simulator(const Configuration& conf_, const char* housePath_ = NULL, const char* algorithmPath_ = NULL);
	~Simulator();

	bool isReady() { return _successful; }
	void simulate();

private:
	void score(int simulationSteps, vector<Simulation*>& simulatios_);
	int getActualPosition(vector<Simulation*>& allSimulatios_, Simulation& currSimulation_) const;
	void printScores() const;
	void printErrors();
	bool endsWith(string value, string ending);

	static int CountSpaces(double avg);

	vector<House*> loadAllHouses(const char* house_path);
	vector<AlgorithmContainer*> loadAllAlgos(const char* algorithm_path);
	vector<string> loadFilesWithSuffix(const char* path, const char* suffix);

	template <class T>
	static void clearPointersVector(vector<T*>& vec);

	template <class T>
	string numberToString(T Number) const;



};


#endif //__SIMULATOR__H_