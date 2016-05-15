# source files and object files
src = main.cpp Simulator.cpp Simulation.cpp ParamsParser.cpp House.cpp Configuration.cpp AlgorithmRegistration.cpp AlgorithmRegistrar.cpp
obj = $(src:.cpp=.o)

# shared object source files and object files
so_src = 201445681_C_.cpp 201445681_B_.cpp 201445681_A_.cpp
so_dep = AlgorithmBase.cpp
so_obj = $(so_src:.cpp=.so)
so_dep_obj = $(so_dep:.cpp=.o)

target = simulator

# flags
SHARED_FLAGS = -O2 -Wall -pthread -std=c++11 -pedantic -g
CC_FLAGS = -c $(SHARED_FLAGS)
SO_FLAGS = $(SHARED_FLAGS) $(LD_FLAGS) -shared -fPIC -mcmodel=large
LD_FLAGS = -lm -lboost_system -lboost_filesystem -ldl
CC = g++

.DEFAULT_GOAL := all
.PHONY: all clean


all: clean $(target)

$(target): $(obj) $(so_obj)
	$(CC) -rdynamic -o $@ $(obj) $(LD_FLAGS)

$(so_obj): %.so: %.cpp $(so_dep_obj)
	$(CC) $(SO_FLAGS) $^ -o $@

$(obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) $< -o $@

$(so_dep_obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) -fPIC $< -o $@
	
clean:
	rm -f $(obj) $(so_obj) $(so_dep_obj) $(target)