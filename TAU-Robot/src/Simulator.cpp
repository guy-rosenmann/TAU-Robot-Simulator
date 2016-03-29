#include "Simulator.h"
#include "Algorithm.h"

#include <algorithm>


Simulator::Simulator(const Configuration& conf_, const char* housePath_)
{
	_config = conf_;
	
	try
	{
		House* h = new House(housePath_);
		_houses.push_back(h);
	}
	catch (...)
	{
		cout << "[ERROR] The given house is not valid. Terminating..." << endl;
	}
	
	
	// create Algorithms
	_algos.push_back(std::make_pair(new Algorithm(), new std::vector<int>()));
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
	
	int maxSteps = _config["MaxSteps"],
		maxStepsAfterWinner = _config["MaxStepsAfterWinner"];

	// for each house, simulate all possible algorithms
	for (vector<House*>::iterator h_it = _houses.begin(); h_it != _houses.end(); ++h_it)
	{
		House& house = **h_it;
		vector<Simulation*> simulatios;
		for (AlgoVector::iterator a_it = _algos.begin(); a_it != _algos.end(); ++a_it)
		{
			AlgoPair& algoPair = *a_it;
			AbstractAlgorithm* algo = algoPair.first;
			//std::vector<int>* algo_scores = algoPair.second;

			simulatios.push_back(new Simulation(_config, house, algo));
		}

#ifdef _DEBUG_
		cout << house << endl;
#endif
		// Simulate all algorithms on current house
		vector<Simulation*> tempSoppedSimulatios;
		bool atLeastOneDone = false;
		int stepsCount = 0, afterStepsCount = -1;
		while ((simulatios.size() > 0) && (stepsCount < maxSteps) && (atLeastOneDone ? (afterStepsCount < maxStepsAfterWinner) : true) )
		{
			vector<Simulation*>::iterator it = simulatios.begin();
			while (it != simulatios.end())
			{
				Simulation& currentSimulation = **it;
				if (!currentSimulation.step() || currentSimulation.isDone())
				{
					if (currentSimulation.isDone())
					{
						atLeastOneDone = true;
					}
					tempSoppedSimulatios.push_back(*it);
					it = simulatios.erase(it);
#ifdef _DEBUG_
					cout << "Simulation is done!" << endl;
					currentSimulation.printStatus();
#endif
				}
				else
				{
					++it;
#ifdef _DEBUG_
					// currentSimulation.printStatus();
#endif
				}
			}

			if (atLeastOneDone)
			{
				afterStepsCount++;
			}
			stepsCount++;
		}
		
#ifdef _DEBUG_
		cout << "[INFO] Total simulation steps for current house: " << stepsCount << endl << endl;
#endif

		simulatios.insert(simulatios.end(), tempSoppedSimulatios.begin(), tempSoppedSimulatios.end());
		tempSoppedSimulatios.clear();
		this->score(stepsCount, simulatios);

		////////////////////////////////////////////////////////////////////////////////////////////
		// print here for EX1 only! (need to print score before any warnings!)
		this->printScores();
		for (vector<Simulation*>::iterator it = simulatios.begin(); it != simulatios.end(); ++it)
		{
			Simulation& curr = **it;
			if (curr.isRobotOutOfBattery())
			{
				cout << "[INFO] The robot was stuck with an empty battery." << endl;
			}
			if (curr.didRobotMisbehave())
			{
				cout << "[INFO] The robot was trying to walk through a wall." << endl;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////

		
		Simulator::clearPointersVector(simulatios);
	}

	//this->printScores(); // TODO: uncomment after EX1, and remove prints above
}


void Simulator::score(int simulationSteps, vector<Simulation*>& simulatios_)
{
	std::sort(simulatios_.begin(), simulatios_.end()); // sort by winner score (done && less steps are first)

	Simulation& firstSim = *simulatios_.at(0);
	int winner_num_steps = firstSim.isDone() ? firstSim.getStepsCount() : simulationSteps;

	for (vector<Simulation*>::iterator it = simulatios_.begin(); it != simulatios_.end(); ++it)
	{
		Simulation& currentSim = **it;
		int index = it - simulatios_.begin();
		
		int position_in_competition = 10;
		if (currentSim.isDone())
		{
			position_in_competition = this->getActualPosition(simulatios_, currentSim);
		}

		_algos[index].second->push_back(currentSim.score(position_in_competition, winner_num_steps, simulationSteps)); // save score
	}
}


int Simulator::getActualPosition(vector<Simulation*>& allSimulatios_, Simulation& currSimulation_) const
{
	int actual_position_in_competition = 1;
	
	// find actual position
	for (vector<Simulation*>::iterator p_it = allSimulatios_.begin(); *p_it != &currSimulation_; ++p_it)
	{
		Simulation& tempSim = **p_it;
		if (p_it == allSimulatios_.begin())
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


void Simulator::printScores() const
{
	for (AlgoVector::const_iterator it = _algos.begin(); it != _algos.end(); ++it)
	{
		std::vector<int>& scores = *it->second;
		for (std::vector<int>::iterator s_it = scores.begin(); s_it != scores.end(); ++s_it)
		{
			int houseIndex = s_it - scores.begin();
			printf("[%s]\t%d\n", _houses[houseIndex]->getName().c_str(), *s_it);
		}
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
