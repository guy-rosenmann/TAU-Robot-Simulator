.DEFAULT_GOAL := simulator

# source files and object files
src = main.cpp Simulator.cpp Simulation.cpp ParamsParser.cpp House.cpp Configuration.cpp Algorithm.cpp
obj = $(src:.cpp=.o)

# flags
CC_FLAGS = -c -O2 -Wall -pthread -std=c++11 -pedantic -g
LD_FLAGS = -lm
CC = g++

all: simulator

simulator: $(obj)
	$(CC) -o $@ $^ $(LD_FLAGS)

$(obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) simulator