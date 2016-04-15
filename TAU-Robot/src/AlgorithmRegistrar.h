#ifndef __ALGORITHM_REGISTRAR__H_
#define __ALGORITHM_REGISTRAR__H_

#include <map>
#include "AbstractAlgorithm.h"

using namespace std;

// typedef to make it easier to set up our factory 
typedef AbstractAlgorithm *maker_t();

// our global factory 
extern map<string, maker_t*> factory;

#endif //__ALGORITHM_REGISTRAR__H_