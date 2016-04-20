# source files and object files
src = main.cpp Simulator.cpp Simulation.cpp ParamsParser.cpp House.cpp Configuration.cpp AlgorithmLoader.cpp
obj = $(src:.cpp=.o)

# shared object source files and object files
so_src = 201445681_C_.cpp 201445681_B_.cpp 201445681_A_.cpp
so_dep = AlgorithmBase.cpp
s_obj = $(so_src:.cpp=.so)

target = simulator

# flags
CC_FLAGS = -c -O2 -Wall -pthread -std=c++11 -pedantic -g
SO_FLAGS = -Wall -pthread -std=c++11 -pedantic -g -shared -fPIC
LD_FLAGS = -lm -lboost_system -lboost_filesystem -ldl
CC = g++

.DEFAULT_GOAL := all
.PHONY: all clean


all: clean $(target)

$(target): $(obj) $(s_obj)
	$(CC) -o $@ $(obj) $(LD_FLAGS)

$(s_obj): %.so: %.cpp $(so_dep)
	$(CC) $(SO_FLAGS) $^ -o $@

$(obj): %.o: %.cpp
	$(CC) $(CC_FLAGS) $< -o $@
	
clean:
	rm -f $(obj) $(s_obj) $(target)