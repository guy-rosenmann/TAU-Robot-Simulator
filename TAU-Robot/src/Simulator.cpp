#include "Simulator.h"
#include "AlgorithmLoader.h"
#include "StringUtils.h"
#include "Constants.h"

#include <algorithm>
#include <boost/filesystem.hpp>

#ifdef _WINDOWS_
#include "201445681_A_.h"
#include "201445681_B_.h"
#include "201445681_C_.h"
#endif

Simulator::Simulator(const Configuration& conf_, const char* housePath_, const char* algorithmPath_)
{
	_config = conf_;
	
	// Handle Alogs
	vector<string> algoErrors;
	if (!getAlgos(algorithmPath_, algoErrors))
	{
		return;
	}
	
	// Handle Houses
	if (!getHouses(housePath_))
	{
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


bool Simulator::getAlgos(const char* algorithmPath_, vector<string>& errors_)
{
	string algoPath = string(algorithmPath_ != NULL ? algorithmPath_ : ".");
	vector<AlgorithmLoader*> allAlgos = loadAllAlgos(algoPath.c_str());

	if (allAlgos.size() == 0)
	{
		cout << USAGE_MSG << endl;
		return false;
	}

	for (AlgorithmLoader* algo : allAlgos)
	{
		if (algo->isValid())
		{
			_algos.push_back(std::make_pair(algo, new std::vector<int>()));
		}
		else
		{
			errors_.push_back(algo->getErrorLine());
			delete algo;
		}
	}

	if (_algos.size() == 0)
	{
		cout << "All algorithm files in target folder '" << boost::filesystem::canonical(algoPath).string() << "' cannot be opened or are invalid: " << endl;

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
		cout << USAGE_MSG << endl;
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
		cout << "All house files in target folder '" << boost::filesystem::canonical(housePath).string() << "' cannot be opened or are invalid: " << endl;
		printErrors();
		return false;
	}

	return true;
}


void Simulator::simulate()
{
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
			string algoName = algoPair.first->GetAlgorithmName();

			// Invoking new instance			
			AbstractAlgorithm* algo = globalFactory[algoName]();
			//std::vector<int>* algo_scores = algoPair.second;

			simulations.push_back(new Simulation(config, house, algo, algoName));
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
		cout << "[INFO] Total simulation steps for current house: " << stepsCount << endl << endl;
#endif

		simulations.insert(simulations.end(), tempStoppedSimulatios.begin(), tempStoppedSimulatios.end());
		tempStoppedSimulatios.clear();
		this->score(stepsCount, simulations);

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
	std::sort(simulations_.begin(), simulations_.end(), Simulation::Compare); // sort by winner score (done && less steps are first)
	
	Simulation& firstSim = *simulations_.at(0);
	int winner_num_steps = firstSim.isDone() ? firstSim.getStepsCount() : simulationSteps;

	for (vector<Simulation*>::iterator it = simulations_.begin(); it != simulations_.end(); ++it)
	{
		Simulation& currentSim = **it;
		
		int position_in_competition = 10;
		if (currentSim.isDone())
		{
			position_in_competition = this->getActualPosition(simulations_, currentSim);
		}

		for (auto it = _algos.begin(); it != _algos.end(); ++it)
		{
			if ((*it).first->GetAlgorithmName() == currentSim.getAlgoName())
			{
				(*it).second->push_back(currentSim.score(position_in_competition, winner_num_steps, simulationSteps)); // save score
				break;
			}
		}
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


vector<AlgorithmLoader*> Simulator::loadAllAlgos(const char* algorithm_path)
{
	vector<AlgorithmLoader*> result;
	vector<string> files = loadFilesWithSuffix(algorithm_path, "_.so");

#ifndef _WINDOWS_
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		result.push_back(new AlgorithmLoader((*it).c_str()));
	}
#else
	// result.push_back(new AlgorithmLoader(new _201445681_A(), "201445681_A_"));
	// result.push_back(new AlgorithmLoader(new _201445681_B(), "201445681_B_"));
	// result.push_back(new AlgorithmLoader(new _201445681_C(), "201445681_C_"));
#endif

	return result;
}


vector<string> Simulator::loadFilesWithSuffix(const char* path_, const char* suffix_)
{
	vector<string> result;
	boost::filesystem::path path(path_);
	if (!is_directory(path))
	{
		return result;
	}

#ifdef _DEBUG_
	std::cout << "[INFO] " << p << " is a directory" << endl;
#endif

	boost::filesystem::directory_iterator end_it;
	for (boost::filesystem::directory_iterator it(path); it != end_it; ++it)
	{
		if ( is_regular_file(*it) && StringUtils::endsWith(it->path().generic_string(), suffix_) )
		{
			result.push_back(it->path().generic_string());

#ifdef _DEBUG_
			std::cout << "[INFO] " << it->path() << " File with " << suffix << " suffix" << endl;
#endif
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
		string filename = boost::filesystem::path((*it)->getFileName()).stem().generic_string();
		cout << filename.substr(0,9) << string(CELL_SIZE - min((int)filename.size(), 9), ' ') << '|';
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

			printf("%10s", StringUtils::numberToString<int>(score).c_str());
			cout << '|';
		}

		avg /= (scores.size());
		int leadingSpaces = CountSpaces(avg);

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
