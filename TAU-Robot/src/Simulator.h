#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <functional>

using namespace std;

#include "Simulation.h"
#include "AlgorithmRegistrar.h"

#define ALGO_NAME_CELL_SIZE 13
#define CELL_SIZE 10


template <class T>
class syncVector
{
	std::vector<T> _vector;
	mutex _mutex;
public:
	syncVector() {}
	syncVector(size_t n) : _vector(n) {}

	void push_back(const T& val) { lock_guard<mutex> lock(_mutex); _vector.push_back(val); }
	void push_back(T&& val) { lock_guard<mutex> lock(_mutex); _vector.push_back(val); }
	void clear() { lock_guard<mutex> lock(_mutex); _vector.clear(); }
	size_t size() { lock_guard<mutex> lock(_mutex); return _vector.size(); }
	
	typename std::vector<T>::const_iterator begin() const { return _vector.begin(); }
	typename std::vector<T>::const_iterator end() const { return _vector.end(); }
};


class Simulator
{
	static string scoreFunctionFileName;

	Configuration	_config;
	vector<House*>	_houses;

	SharedObjectLoader*	_scoreSO = nullptr;
	typedef int(*score_func)(const map<string, int>&);
	score_func	_scoreFunc = NULL;
	bool		_printScoreError = false;
	
	size_t			_threadsCount;

	bool				_successful = false;
	syncVector<string>	_errors;

	map<string, unique_ptr<vector<int>>>	_algoScores;

	atomic_size_t	_houseIndex{0};
	mutex			_algoScoresMutex;
	
public:
	Simulator(const Configuration& conf_, const char* housePath_ = NULL, const char* algorithmPath_ = NULL, const char* scorePath_ = NULL, const char* threadsCount_ = NULL);
	~Simulator();

	bool isReady() { return _successful; }
	void simulate();

private:
	void score(int houseIndex_, int simulationSteps_, vector<Simulation*>& simulatios_);
	int getActualPosition(vector<Simulation*>& allSimulatios_, Simulation& simulationToScore_) const;
	void printScores() const;
	void printErrors() const;

	static int CountSpaces(double avg);

	vector<House*> loadAllHouses(const char* house_path);
	vector<string> loadFilesWithSuffix(const char* path, const char* suffix);

	bool getAlgos(const char* algorithmPath_, vector<string>& errors_);
	bool getHouses(const char* housePath_);
	bool getScoreFunc(const char* scorePath_);

	size_t getThreadsFromString(const char* threads_count) const;
	void simulateOnHouse(int maxStepsAfterWinner, int index, House& house);
	void runSingleSubSimulationThread(int maxStepsAfterWinner);
	template <class T>
	static void clearPointersVector(vector<T*>& vec);
};


#ifdef _DEBUG_
// for Windows tests only (debug prints)
class sync_cout
{
	static mutex _mutex;
	static sync_cout _self;
public:
	template <class T> sync_cout& operator<<(const T& obj) { lock_guard<mutex> lock(_mutex); std::cout << obj; return _self; }
	sync_cout& operator<<(std::ostream& (*f)(std::ostream&)) { lock_guard<mutex> lock(_mutex); std::cout << f; return _self; }
	static sync_cout& get() { return _self; }
};
#endif


#endif //__SIMULATOR__H_