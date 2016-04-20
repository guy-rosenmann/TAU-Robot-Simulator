# source files and object files
src = main.cpp Simulator.cpp Simulation.cpp ParamsParser.cpp House.cpp Configuration.cpp AlgorithmLoader.cpp
obj = $(src:.cpp=.o)
target = simulator

# flags
CC_FLAGS = -c -O2 -Wall -pthread -std=c++11 -pedantic -g
LD_FLAGS = -lm -lboost_system -lboost_filesystem -ldl
CC = g++

.DEFAULT_GOAL := all
.PHONY: all clean


all: clean $(target)

$(target): $(obj)
	$(CC) -o $@ $^ $(LD_FLAGS)

$(obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) $< -o $@
	
clean:
	rm -f $(obj) $(target)