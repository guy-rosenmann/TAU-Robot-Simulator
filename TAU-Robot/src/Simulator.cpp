#include "Simulator.h"
#include "StringUtils.h"
#include "ParamsParser.h"
#include "AlgorithmRegistrar.h"
#include "MakeUnique.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <thread>

namespace fs = boost::filesystem;


#ifdef _DEBUG_
// for Windows tests only (debug prints)
mutex sync_cout::_mutex;
sync_cout sync_cout::_self;
#endif


string Simulator::scoreFunctionFileName = "score_formula.so";

Simulator::Simulator(const Configuration& conf_, const char* housePath_, const char* algorithmPath_, const char* scorePath_, const char* threadsCount_)
{
	_config = conf_;

	//get score function
	if (!getScoreFunc(scorePath_))
	{
		return;
	}
	
	// get threads
	size_t requestedThreadsCount = getThreadsFromString(threadsCount_);

	// Handle Alogs
	vector<string> algoErrors;
#ifndef _WINDOWS_
	if (!getAlgos(algorithmPath_, algoErrors))
	{
		return;
	}
#endif

	// Handle Houses
	if (!getHouses(housePath_))
	{
		return;
	}

	// Create scores matrix
	vector<string> algoNames = AlgorithmRegistrar::getInstance().getAlgorithmNames();
	for (vector<string>::iterator it = algoNames.begin(); it != algoNames.end(); ++it)
	{
		_algoScores[*it] = make_unique<vector<int>>(_houses.size());
	}

	// Concatenate algo errors to house errors
	for (vector<string>::iterator it = algoErrors.begin(); it != algoErrors.end(); ++it)
	{
		_errors.push_back(*it);
	}

	
	_threadsCount = min(requestedThreadsCount, _houses.size());
#ifdef _DEBUG_
	sync_cout::get() << "_threadsCount: " << _threadsCount << endl;
#endif	

	_successful = true;
}

Simulator::~Simulator()
{
	Simulator::clearPointersVector(_houses);

	if (_scoreSO != nullptr)
	{
		delete _scoreSO;
	}
}


bool Simulator::getAlgos(const char* algorithmPath_, vector<string>& errors_)
{
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
	
	string algoPath = string(algorithmPath_ != NULL ? algorithmPath_ : ".");
	vector<string> files = loadFilesWithSuffix(algoPath.c_str(), "_.so");

	if (files.size() == 0)
	{
		ParamsParser::printUsage();
		cout << "cannot find algorithm files in '" << StringUtils::getFullPath(algoPath) << "'" << endl;
		return false;
	}

	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		int err = registrar.loadAlgorithm((*it).c_str());
		if (err != AlgorithmRegistrar::ALGORITHM_REGISTERED_SUCCESSFULY)
		{
			string fileName = fs::path((*it).c_str()).filename().generic_string();
			if (err == AlgorithmRegistrar::FILE_CANNOT_BE_LOADED)
			{
				errors_.push_back(fileName + ": file cannot be loaded or is not a valid .so");
			}
			else if (err == AlgorithmRegistrar::NO_ALGORITHM_REGISTERED)
			{
				errors_.push_back(fileName + ": valid .so file but no algorithm was registered after loading it");
			}
		}
	}

	if (registrar.size() == 0)
	{
		cout << "All algorithm files in target folder '" << StringUtils::getFullPath(algoPath) << "' cannot be opened or are invalid: " << endl;

		// Only algorithms errors should be printed
		for (vector<string>::iterator it = errors_.begin(); it != errors_.end(); ++it)
		{
			cout << (*it) << endl;
		}

		return false;
	}
	return true;
}


bool Simulator::getHouses(const char* housePath_)
{
	string housePath = string(housePath_ != NULL ? housePath_ : ".");
	vector<House*> allHouses = loadAllHouses(housePath.c_str());
	
	if (allHouses.size() == 0)
	{
		ParamsParser::printUsage();
		cout << "cannot find house files in '" << StringUtils::getFullPath(housePath) << "'" << endl;
		return false;
	}

	for (House* house : allHouses)
	{
		if (house->isValid())
		{
			_houses.push_back(house);
		}
		else
		{
			_errors.push_back(house->getErrorLine());
			delete house;
		}
	}

	if (_houses.size() == 0)
	{
		cout << "All house files in target folder '" << StringUtils::getFullPath(housePath) << "' cannot be opened or are invalid: " << endl;
		printErrors();
		return false;
	}

	return true;
}


bool Simulator::getScoreFunc(const char* scorePath_)
{
	if (scorePath_ != NULL)
	{
		string scoreFile = StringUtils::getWithTrailingSlash(scorePath_) + Simulator::scoreFunctionFileName;	
		if (!fs::exists(scoreFile.c_str()) || fs::is_directory(fs::path(scoreFile)))
		{
			ParamsParser::printUsage();
			cout << "cannot find score_formula.so file in '" << StringUtils::getFullPath(scorePath_) << "'" << endl;
			return false;
		}

		_scoreSO = new SharedObjectLoader(scoreFile.c_str());
		if (_scoreSO->isValid())
		{
			_scoreFunc = reinterpret_cast<score_func>( reinterpret_cast<long>( _scoreSO->getFunctionPointer("calc_score") ) );
			if (_scoreFunc == NULL)
			{
				cout << "score_formula.so is a valid .so but it does not have a valid score formula" << endl;
				return false;
			}
		}
		else
		{
			cout << "score_formula.so exists in '" << StringUtils::getFullPath(scorePath_) << "' but cannot be opened or is not a valid .so" << endl;
			return false;
		}
	}
	else
	{
		_scoreFunc = Simulation::calc_score;
	}

	return true;
}


size_t Simulator::getThreadsFromString(const char* threads_count) const
{
	int num;

	if (threads_count == NULL)
	{
		return 1;
	}

	try
	{
		num = stoi(threads_count);
	}
	catch (std::invalid_argument e)
	{
		return 1;
	}
	catch (std::out_of_range e)
	{
		return 1;
	}

	if (num <= 0)
	{
		return 1;
	}

	return num;
}

void Simulator::simulateOnHouse(int maxStepsAfterWinner, int index, House& house)
{
	vector<Simulation*> simulations;

#ifdef _DEBUG_
	sync_cout::get() << std::this_thread::get_id() << " running on house: " << index << endl << endl;
#endif

	// We need to set MaxSteps for each house sepreratly
	Configuration config(_config);
	int maxSteps = house.getMaxSteps();

	map<string, unique_ptr<AbstractAlgorithm>> algorithms = AlgorithmRegistrar::getInstance().getAlgorithms();
	for (auto a_it = algorithms.begin(); a_it != algorithms.end(); ++a_it)
	{
		simulations.push_back(new Simulation(config, house, a_it->second, a_it->first));
	}
#ifdef _DEBUG_
	sync_cout::get() << house << endl;
#endif
	// Simulate all algorithms on current house
	vector<Simulation*> tempStoppedSimulatios;
	bool atLeastOneDone = false, aboutToFinishCalled = false;
	int stepsCount = 0, afterStepsCount = -1;
	while ((simulations.size() > 0) && (stepsCount < maxSteps) && (atLeastOneDone ? (afterStepsCount < maxStepsAfterWinner) : true) )
	{
		vector<Simulation*>::iterator it = simulations.begin();
		while (it != simulations.end())
		{
			Simulation& currentSimulation = **it;

			if (!currentSimulation.step() || currentSimulation.isDone())
			{
				if (currentSimulation.isDone())
				{
					atLeastOneDone = true;
				}
				if (currentSimulation.didRobotMisbehave())
				{
					_errors.push_back(currentSimulation.getAlgoName() + " when running on House " + house.getFilenameWithoutSuffix() + " went on a wall in step " + to_string(stepsCount+1));
				}
				tempStoppedSimulatios.push_back(*it);
				it = simulations.erase(it);
//#ifdef _DEBUG_
//				cout << "Simulation is done!" << endl;
//				if (stepsCount % 5 == 0)
//				{
//					currentSimulation.printStatus();
//				}
//#endif
			}
			else
			{
				++it;
#ifdef _DEBUG_
				// currentSimulation.printStatus();
				if (stepsCount == maxSteps - 1)
				{
					sync_cout::get() << "Simulation is stopped!" << endl;
				}
//				currentSimulation.printStatus();
#endif
			}
		}

		if (atLeastOneDone)
		{
			afterStepsCount++;
		}
		stepsCount++;

		// Calling aboutToFinish only once per algorithm
		// According to the forum there are 2 scenarios for our call to aboutToFinish()
		//		1. No winner and (stepsCount >= maxSteps - maxStepsAfterWinner)
		//		2. Winner finished and aboutToFinish() was not called
		if (!aboutToFinishCalled && (atLeastOneDone || (stepsCount == maxSteps - maxStepsAfterWinner)))
		{
			aboutToFinishCalled = true;

			// Iterating on all active simulations and calling aboutToFinish()
			for (vector<Simulation*>::iterator itt = simulations.begin(); itt != simulations.end(); ++itt)
			{
				(*itt)->CallAboutToFinish(min(maxSteps - stepsCount, maxStepsAfterWinner));
			}
		}
	}
		
#ifdef _DEBUG_
	sync_cout::get() << "[INFO] Total simulation steps for current house: " << stepsCount << endl << endl;
#endif
	simulations.insert(simulations.end(), tempStoppedSimulatios.begin(), tempStoppedSimulatios.end());
	tempStoppedSimulatios.clear();
	this->score(index, stepsCount, simulations);

	Simulator::clearPointersVector(simulations);
}

void Simulator::runSingleSubSimulationThread(int maxStepsAfterWinner) {
	// ===> thread should take a new task, if available, and run it
	// if no task is available, thread is done
#ifdef _DEBUG_
	sync_cout::get() << std::this_thread::get_id() << " runSingleSubSimulationThread " << endl;
#endif

	for (size_t index = _houseIndex++; index < _houses.size(); index = _houseIndex++) // fetch old value, then add. equivalent to: fetch_add(1)
	{
		House& house = *_houses.at(index);
		simulateOnHouse(maxStepsAfterWinner, index, house);
	}
}

void Simulator::simulate()
{
	if (!_successful) return;
	int maxStepsAfterWinner = _config["MaxStepsAfterWinner"];

	vector<unique_ptr<thread>> threads(_threadsCount);
	for (auto& thread_ptr : threads)
	{
		// ===> actually create the threads and run them
		thread_ptr = make_unique<thread>(&Simulator::runSingleSubSimulationThread, this, maxStepsAfterWinner); // create and run the thread
	}

	// ===> join all the threads to finish nicely (i.e. without crashing / terminating threads)
	for (auto& thread_ptr : threads)
	{
		thread_ptr->join();
	}

	this->printScores();

	if (_errors.size() > 0)
	{
		cout << endl << "Errors:" << endl;
		printErrors();
	}
}


void Simulator::score(int houseIndex_, int simulationSteps_, vector<Simulation*>& simulations_)
{
	std::sort(simulations_.begin(), simulations_.end(), Simulation::Compare); // sort by winner score (done && less steps are first)
	
	Simulation& firstSim = *simulations_.at(0);
	int winner_num_steps = firstSim.isDone() ? firstSim.getStepsCount() : simulationSteps_;

	for (vector<Simulation*>::iterator it = simulations_.begin(); it != simulations_.end(); ++it)
	{
		Simulation& currentSim = **it;
		
		map<string, int> scoreParams;
		scoreParams["actual_position_in_competition"] = this->getActualPosition(simulations_, currentSim);
		scoreParams["simulation_steps"] = simulationSteps_;
		scoreParams["winner_num_steps"] = winner_num_steps;
		scoreParams["this_num_steps"] = currentSim.isRobotOutOfBattery() ? simulationSteps_ : currentSim.getStepsCount();
		scoreParams["sum_dirt_in_house"] = currentSim.getTotalDirtCount();
		scoreParams["dirt_collected"] = currentSim.getCleanedDirtCount();
		scoreParams["is_back_in_docking"] = currentSim.isRobotDocked() ? 1 : 0;
		scoreParams["did_misbehave"] = currentSim.didRobotMisbehave() ? 1 : 0;

		lock_guard<mutex> lock(_algoScoresMutex); // this lock will prevent parallel writes to _algoScores (freed when out of scope)
		int currScore = _scoreFunc(scoreParams);
		if (currScore == -1 && !_wasScoreErrorPrinted)
		{
			_errors.push_back("Score formula could not calculate some scores, see -1 in the results table");
			_wasScoreErrorPrinted = true;
		}
		(*_algoScores[currentSim.getAlgoName()])[houseIndex_] = currScore;
		
	}
}


// Assumes: allSimulations_ is sorted
int Simulator::getActualPosition(vector<Simulation*>& allSimulations_, Simulation& simulationToScore_) const
{
	// find actual position
	int actual_position_in_competition = 1, sameCount = 1;
	for (vector<Simulation*>::iterator p_it = allSimulations_.begin(); p_it != allSimulations_.end(); ++p_it)
	{
		Simulation& currSim = **p_it;
		
		if (p_it != allSimulations_.begin())
		{
			Simulation& prevSim = **(p_it - 1);
			if (currSim.getStepsCount() != prevSim.getStepsCount())
			{
				actual_position_in_competition += sameCount;
				sameCount = 1;
			}
			else
			{
				sameCount++;
			}
		}

		if (&currSim == &simulationToScore_ || !currSim.isDone()) break;
	}

	return actual_position_in_competition;
}


int Simulator::CountSpaces(double avg)
{
	int leadingSpaces = avg < 0 ? CELL_SIZE - 4 : CELL_SIZE - 3, num = (int)avg;
	do
	{
		num /= 10;
		leadingSpaces--;
	} while (num != 0);

	return leadingSpaces;
}


vector<House*> Simulator::loadAllHouses(const char* house_path)
{
	vector<House*> result;
	vector<string> files = loadFilesWithSuffix(house_path, ".house");

	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		result.push_back(new House((*it).c_str()));
	}

	return result;
}



vector<string> Simulator::loadFilesWithSuffix(const char* path_, const char* suffix_)
{
	vector<string> result;
	fs::path path(path_);
	if (!fs::is_directory(path))
	{
		return result;
	}

//#ifdef _DEBUG_
//	sync_cout::get() << "[INFO] " << p << " is a directory" << endl;
//#endif

	fs::directory_iterator end_it;
	for (fs::directory_iterator it(path); it != end_it; ++it)
	{
		if ( is_regular_file(*it) && StringUtils::endsWith(it->path().generic_string(), suffix_) )
		{
			result.push_back(it->path().generic_string());

//#ifdef _DEBUG_
////			sync_cout::get() << "[INFO] " << it->path() << " File with " << suffix << " suffix" << endl;
//#endif
		}
	}
	std::sort(result.begin(), result.end());
	return result;
}


void Simulator::printScores() const
{
	int rowLength = 2 + ALGO_NAME_CELL_SIZE + (1 + _houses.size()) * (CELL_SIZE + 1);

	//////////////////////////////////////
	//////////Print Table First Row///////
	//////////////////////////////////////
	cout << string(rowLength, '-') << endl;
	cout << '|' << string(ALGO_NAME_CELL_SIZE, ' ') << '|';
	for (vector<House*>::const_iterator it = _houses.begin(); it != _houses.end(); ++it)
	{
		string filename = (*it)->getFilenameWithoutSuffix();
		cout << filename.substr(0,9) << string(CELL_SIZE - min((int)filename.size(), 9), ' ') << '|';
	}
	cout << "AVG" << string(CELL_SIZE - 3, ' ') << '|' << endl;
	cout << string(rowLength, '-') << endl;

	//////////////////////////////////////
	//////Print Table Algos' Results//////
	//////////////////////////////////////
	for (auto it = _algoScores.begin(); it != _algoScores.end(); ++it)
	{
		cout << '|';
		string filename = it->first;
		cout << filename.substr(0, ALGO_NAME_CELL_SIZE) << string(std::max(ALGO_NAME_CELL_SIZE - (int)filename.size(), 0), ' ') << '|';
		
		vector<int>& scores = *(it->second);
		double avg = 0.0;
		for (vector<int>::iterator s_it = scores.begin(); s_it != scores.end(); ++s_it)
		{
			int score = *s_it;
			avg += score;

			printf("%10s", StringUtils::numberToString<int>(score).c_str());
			cout << '|';
		}

		avg /= (scores.size());
		int leadingSpaces = Simulator::CountSpaces(avg);

		printf("%s%.2f|", string(leadingSpaces, ' ').c_str(), avg);

		cout << endl;
		cout << string(rowLength, '-') << endl;
	}
}


void Simulator::printErrors() const
{
	for (string error : _errors)
	{
		cout << error << endl;
	}
}


template <typename T>
void Simulator::clearPointersVector(vector<T*>& vec_)
{
	for (typename vector<T*>::iterator it = vec_.begin(); it != vec_.end(); ++it)
	{
		delete *it;
	}
	vec_.clear();
}
