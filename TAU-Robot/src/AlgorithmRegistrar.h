#ifndef __ALGORITHM_REGISTRAR__H_
#define __ALGORITHM_REGISTRAR__H_

#include <map>
#include "AbstractAlgorithm.h"

using namespace std;

typedef AbstractAlgorithm* (*instanceCreator)();

extern map<string, instanceCreator> globalFactory;

#endif //__ALGORITHM_REGISTRAR__H_