#include "Simulator.h"
#include "AlgorithmContainer.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <sstream>
#include "Algorithm_A.h"
#include "Algorithm_B.h"
#include "Algorithm_C.h"


Simulator::Simulator(const Configuration& conf_, const char* housePath_, const char* algorithmPath_)
{
	_config = conf_;
	
	// Handle Alogs
	string algoPath = string(algorithmPath_ != NULL ? algorithmPath_ : ".");
	vector<AlgorithmContainer*> allAlgos = loadAllAlgos(algoPath.c_str());
	vector<string>	algoErrors;

	if (allAlgos.size() == 0)
	{
		cout << "USAGE:\tsimulator [-config <path>] [-house_path <path>] [-algorithm_path <path>]" << endl;
		cout << "\t- algorithm_path:\talgorithm dir path" << endl;
		cout << "\t- default value:\tcurrent dir" << endl;
		return;
	}

	for (AlgorithmContainer* algo : allAlgos)
	{
		if (algo->isValid())
		{
			_algos.push_back(std::make_pair(algo, new std::vector<int>()));
		}
		else
		{
			cout << "New Alg Error: " << algo->getErrorLine() << endl;
			algoErrors.push_back(algo->getErrorLine());
		}
	}

	if (_algos.size() == 0)
	{
		cout << "All algorithm files in target folder " << boost::filesystem::canonical(algoPath) << " cannot be opened or are invalid : " << endl;

		// Only algorithms errors should be printed
		for (vector<string>::iterator it = algoErrors.begin(); it != algoErrors.end(); ++it)
		{
			cout << (*it) << endl;
		}
		return;
	}

//	_algos.push_back(make_pair(new AlgorithmContainer(new Algorithm_A(), "201445681_A_"), new std::vector<int>()));
	_algos.push_back(make_pair(new AlgorithmContainer(new Algorithm_B(), "201445681_B_"), new std::vector<int>()));
	_algos.push_back(make_pair(new AlgorithmContainer(new Algorithm_C(), "201445681_C_"), new std::vector<int>()));
	
	// Handle Houses
	string housePath = string(housePath_ != NULL ? housePath_ : ".");
	vector<House*> allHouses = loadAllHouses(housePath.c_str());
	if (allHouses.size() == 0)
	{
		cout << "USAGE:\tsimulator [-config <path>] [-house_path <path>] [-algorithm_path <path>]" << endl;
		cout << "\t- house_path:\thouse dir path" << endl;
		cout << "\t- default value:\tcurrent dir" << endl;
		return;
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
		}
	}

	if (_houses.size() == 0)
	{
		cout << "All house files in target folder " << boost::filesystem::canonical(housePath) << " cannot be opened or are invalid : " << endl;
		printErrors();
		return;
	}

	// Concatenate algo errors to house errors
	for (vector<string>::iterator it = algoErrors.begin(); it != algoErrors.end(); ++it)
	{
		_errors.push_back(*it);
	}

	_successful = true;
}

Simulator::~Simulator()
{
	Simulator::clearPointersVector(_houses);

	for (AlgoVector::iterator it = _algos.begin(); it != _algos.end(); ++it)
	{
		delete it->first;	// algorithm
		delete it->second;	// scores vector
	}
}


void Simulator::simulate()
{
	if (_houses.size() == 0) return;
	
	int maxStepsAfterWinner = _config["MaxStepsAfterWinner"];

	// for each house, simulate all possible algorithms
	for (vector<House*>::iterator h_it = _houses.begin(); h_it != _houses.end(); ++h_it)
	{
		House& house = **h_it;
		vector<Simulation*> simulations;

		// We need to set MaxSteps for each house sepreratly
		Configuration config(_config);
		int maxSteps = house.getMaxSteps();

		for (AlgoVector::iterator a_it = _algos.begin(); a_it != _algos.end(); ++a_it)
		{
			AlgoPair& algoPair = *a_it;
			AbstractAlgorithm* algo = algoPair.first->getAlgorithm();
			//std::vector<int>* algo_scores = algoPair.second;

			simulations.push_back(new Simulation(config, house, algo));
		}

#ifdef _DEBUG_
		cout << house << endl;
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
					tempStoppedSimulatios.push_back(*it);
					it = simulations.erase(it);
#ifdef _DEBUG_
					cout << "Simulation is done!" << endl;
					if (stepsCount % 5 == 0)
					{
						currentSimulation.printStatus();
					}
#endif
				}
				else
				{
					++it;
#ifdef _DEBUG_
					// currentSimulation.printStatus();
					if (stepsCount == maxSteps - 1)
					{
						cout << "Simulation is stopped!" << endl;
					}
					currentSimulation.printStatus();

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
			if (!aboutToFinishCalled && (atLeastOneDone || (stepsCount >= maxSteps - maxStepsAfterWinner)))
			{
				aboutToFinishCalled = true;

				// Iterating on all active simulations and calling aboutToFinish()
				vector<Simulation*>::iterator iterator = simulations.begin();
				while (iterator != simulations.end())
				{
					(*iterator)->CallAboutToFinish(maxStepsAfterWinner);
					++iterator;
				}
			}
		}
		
#ifdef _DEBUG_
		cout << "[INFO] Total simulation steps for current house: " << stepsCount << endl << endl;
#endif

		simulations.insert(simulations.end(), tempStoppedSimulatios.begin(), tempStoppedSimulatios.end());
		tempStoppedSimulatios.clear();
		this->score(stepsCount, simulations);

//		////////////////////////////////////////////////////////////////////////////////////////////
//		// print here for EX1 only! (need to print score before any warnings!)
//		this->printScores();
//		for (vector<Simulation*>::iterator it = simulatios.begin(); it != simulatios.end(); ++it)
//		{
//			Simulation& curr = **it;
//			if (curr.isRobotOutOfBattery())
//			{
//				cout << "[INFO] The robot was stuck with an empty battery." << endl;
//			}
//			if (curr.didRobotMisbehave())
//			{
//				cout << "[INFO] The robot was trying to walk through a wall." << endl;
//			}
//		}
//		////////////////////////////////////////////////////////////////////////////////////////////
//
//		
		Simulator::clearPointersVector(simulations);
	}

	this->printScores();

	if (_errors.size() > 0)
	{
		cout << endl << "Errors:" << endl;
		printErrors();
	}
}


void Simulator::score(int simulationSteps, vector<Simulation*>& simulations_)
{
	std::sort(simulations_.begin(), simulations_.end()); // sort by winner score (done && less steps are first)

	Simulation& firstSim = *simulations_.at(0);
	int winner_num_steps = firstSim.isDone() ? firstSim.getStepsCount() : simulationSteps;

	for (vector<Simulation*>::iterator it = simulations_.begin(); it != simulations_.end(); ++it)
	{
		Simulation& currentSim = **it;
		int index = it - simulations_.begin();
		
		int position_in_competition = 10;
		if (currentSim.isDone())
		{
			position_in_competition = this->getActualPosition(simulations_, currentSim);
		}

		_algos[index].second->push_back(currentSim.score(position_in_competition, winner_num_steps, simulationSteps)); // save score
	}
}


int Simulator::getActualPosition(vector<Simulation*>& allSimulations_, Simulation& currSimulation_) const
{
	int actual_position_in_competition = 1;
	
	// find actual position
	for (vector<Simulation*>::iterator p_it = allSimulations_.begin(); *p_it != &currSimulation_; ++p_it)
	{
		Simulation& tempSim = **p_it;
		if (p_it == allSimulations_.begin())
		{
			if (tempSim.getStepsCount() < currSimulation_.getStepsCount())
			{
				actual_position_in_competition++;
			}
			else
			{
				break; // have the same score -> all until current have the same score
			}
		}
		else
		{
			if (tempSim.getStepsCount() != (*(p_it - 1))->getStepsCount())
			{
				actual_position_in_competition++;
				if (actual_position_in_competition >= 4)
				{
					break;
				}
			}
		}
	}

	return std::min(actual_position_in_competition, 4);
}


int Simulator::CountSpaces(double avg)
{
	int leadingSpaces = 7, num = (int) avg;
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

vector<AlgorithmContainer*> Simulator::loadAllAlgos(const char* algorithm_path)
{
	vector<AlgorithmContainer*> result;
	vector<string> files = loadFilesWithSuffix(algorithm_path, "_.so");

	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		result.push_back(new AlgorithmContainer((*it).c_str()));
	}

	return result;
}

vector<string> Simulator::loadFilesWithSuffix(const char* path, const char* suffix)
{
	vector<string> result;
	boost::filesystem::path p(path);
	if (!is_directory(p))
	{
		return result;
	}

#ifdef _DEBUG_
	std::cout << "[INFO] " << p << " is a directory" << endl;
#endif

	boost::filesystem::directory_iterator it(p);
	boost::filesystem::directory_iterator endit;

	while (it != endit)
	{
		if (is_regular_file(*it) && endsWith(it->path().generic_string(), suffix)){
			result.push_back(it->path().generic_string());

#ifdef _DEBUG_
			std::cout << "[INFO] " << it->path() << " File with " << suffix << " suffix" << endl;
#endif
		}
		++it;
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
		string filename = boost::filesystem::path((*it)->getFileName()).stem().generic_string();
		cout << filename << string(CELL_SIZE - filename.size(), ' ') << '|';
	}
	cout << "AVG" << string(CELL_SIZE - 3, ' ') << '|' << endl;
	cout << string(rowLength, '-') << endl;

	//////////////////////////////////////
	//////Print Table Algos' Results//////
	//////////////////////////////////////
	for (AlgoVector::const_iterator it = _algos.begin(); it != _algos.end(); ++it)
	{
		cout << '|';
		string filename = (*it).first->GetAlgorithmName();
		cout << filename << string(ALGO_NAME_CELL_SIZE - filename.size(), ' ') << '|';
		
		vector<int>& scores = *it->second;
		double avg = 0.0;
		for (vector<int>::iterator s_it = scores.begin(); s_it != scores.end(); ++s_it)
		{
			int score = *s_it;
			avg += score;

			printf("%10s", numberToString<int>(score).c_str());
			cout << '|';
		}

		avg /= (scores.size());
		int leadingSpaces = CountSpaces(avg);

		printf("%s%.2f|", string(leadingSpaces, ' ').c_str(), avg);

		cout << endl;
		cout << string(rowLength, '-') << endl;
	}
}


bool Simulator::endsWith(string value, string ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void Simulator::printErrors()
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

template <typename T>
string Simulator::numberToString(T num) const
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}
