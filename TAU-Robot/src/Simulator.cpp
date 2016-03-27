#include "Simulator.h"
#include "Algorithm.h"

#include <algorithm>


Simulator::Simulator(const Configuration& conf_)
{
	_config = conf_;
	_houses.push_back(new House());
	
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
	int maxSteps = _config["MaxSteps"],
		maxStepsAfterWinner = _config["MaxStepsAfterWinner"];

	// for each house, simulate all possible algorithms
	for (vector<House*>::iterator h_it = _houses.begin(); h_it != _houses.end(); ++h_it)
	{
		House& house = **h_it;
		cout << house;
		vector<Simulation*> simulatios;
		for (AlgoVector::iterator a_it = _algos.begin(); a_it != _algos.end(); ++a_it)
		{
			AlgoPair& algoPair = *a_it;
			AbstractAlgorithm* algo = algoPair.first;
			//std::vector<int>* algo_scores = algoPair.second;

			simulatios.push_back(new Simulation(_config, house, algo));
		}

		// Simulate all algorithms on current house
		vector<Simulation*> tempSoppedSimulatios;
		bool atLeastOneDone = false;
		int stepsCount = 0, afterStepsCount = -1;
		while ((simulatios.size() > 0) &&
			(atLeastOneDone ? ((stepsCount < maxSteps) && (afterStepsCount < maxStepsAfterWinner)) : (stepsCount < maxSteps)))
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
				}
				else
				{
					++it;
				}
			}

			if (atLeastOneDone)
			{
				afterStepsCount++;
			}
			stepsCount++;
		}
		
		simulatios.insert(simulatios.end(), tempSoppedSimulatios.begin(), tempSoppedSimulatios.end());
		tempSoppedSimulatios.clear();
		this->score(stepsCount, simulatios);

		Simulator::clearPointersVector(simulatios);
	}

	// print results
	for (AlgoVector::iterator it = _algos.begin(); it != _algos.end(); ++it)
	{
		std::vector<int>& scores = *it->second;
		for (std::vector<int>::iterator s_it = scores.begin(); s_it != scores.end(); ++s_it)
		{
			int houseIndex = s_it - scores.begin();
			printf("[%s]\t%d\n", _houses[houseIndex]->getName().c_str(), *s_it);
		}
	}
}


void Simulator::score(int simulationSteps, vector<Simulation*>& simulatios_)
{
	std::sort(simulatios_.begin(), simulatios_.end()); // sort by winner score (done && less steps)

	Simulation& firstSim = *simulatios_.at(0);
	int winner_num_steps = firstSim.isDone() ? firstSim.getStepsCount() : simulationSteps;

	for (vector<Simulation*>::iterator it = simulatios_.begin(); it != simulatios_.end(); ++it)
	{
		Simulation& currentSim = **it;
		int index = it - simulatios_.begin();
		
		int position_in_copmetition = 10;
		int actual_position_in_copmetition = 1;
		if (currentSim.isDone())
		{
			// find actual position
			for (vector<Simulation*>::iterator p_it = simulatios_.begin(); p_it != it; ++p_it)
			{
				Simulation& tempSim = **p_it;
				if (p_it == simulatios_.begin())
				{
					if (tempSim.getStepsCount() < currentSim.getStepsCount())
					{
						actual_position_in_copmetition++;
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
						actual_position_in_copmetition++;
						if (actual_position_in_copmetition >= 4)
						{
							break;
						}
					}
				}
			}

			position_in_copmetition = std::min(actual_position_in_copmetition, 4);
		}

		_algos[index].second->push_back(currentSim.score(position_in_copmetition, winner_num_steps, simulationSteps)); // save score
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
