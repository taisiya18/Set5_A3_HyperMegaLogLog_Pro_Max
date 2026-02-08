CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SOURCES = RandomStreamGen.cpp HashFuncGen.cpp HyperLogLog.cpp
HEADERS = RandomStreamGen.h HashFuncGen.h HyperLogLog.h
OBJECTS = $(SOURCES:.cpp=.o)

TEST1_EXEC = test_stage1
TEST2_EXEC = test_stage2

all: $(TEST1_EXEC) $(TEST2_EXEC)

$(TEST1_EXEC): test_stage1.o RandomStreamGen.o HashFuncGen.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST2_EXEC): test_stage2.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TEST1_EXEC) $(TEST2_EXEC) *.csv *.png

run1: $(TEST1_EXEC)
	./$(TEST1_EXEC)

run2: $(TEST2_EXEC)
	./$(TEST2_EXEC)

visualize: run2
	python3 visualize.py

test: all
	./$(TEST1_EXEC)
	./$(TEST2_EXEC)
	python3 visualize.py

.PHONY: all clean run1 run2 visualize test
