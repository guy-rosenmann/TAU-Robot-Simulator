#include "AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory_)
{
	AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory_);
}