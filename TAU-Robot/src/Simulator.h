#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>

using namespace std;

#include "Simulation.h"
#include "AlgorithmRegistrar.h"

#define ALGO_NAME_CELL_SIZE 13
#define CELL_SIZE 10

class Simulator
{
	Configuration	_config;
	vector<House*>	_houses;
	size_t _threadsCount;

	bool			_successful = false;
	vector<string>	_errors;

	map<string, unique_ptr<vector<int>>>	_algoScores;
	mutex _algoScoresMutex;

	atomic_size_t _houseIndex{0};
	
public:
	Simulator(const Configuration& conf_, const char* housePath_ = NULL, const char* algorithmPath_ = NULL, const char* threadsCount_ = NULL);
	~Simulator();

	bool isReady() { return _successful; }
	void simulate();

private:
	void score(int houseIndex_, int simulationSteps_, vector<Simulation*>& simulatios_);
	int getActualPosition(vector<Simulation*>& allSimulatios_, Simulation& currSimulation_) const;
	void printScores() const;
	void printErrors() const;

	static int CountSpaces(double avg);

	vector<House*> loadAllHouses(const char* house_path);
	vector<string> loadFilesWithSuffix(const char* path, const char* suffix);

	bool getAlgos(const char* algorithmPath_, vector<string>& errors_);
	bool getHouses(const char* housePath_);

	size_t getThreadsFromString(const char* threads_count) const;
	void simulateOnHouse(int maxStepsAfterWinner, int index, House& house);
	void runSingleSubSimulationThread(int maxStepsAfterWinner);
	template <class T>
	static void clearPointersVector(vector<T*>& vec);
};


#endif //__SIMULATOR__H_