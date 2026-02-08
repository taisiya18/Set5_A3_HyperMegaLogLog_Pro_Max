CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SOURCES = RandomStreamGen.cpp HashFuncGen.cpp
HEADERS = RandomStreamGen.h HashFuncGen.h
OBJECTS = $(SOURCES:.cpp=.o)

TEST_EXEC = test_stage1

all: $(TEST_EXEC)

$(TEST_EXEC): test_stage1.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TEST_EXEC)

run: $(TEST_EXEC)
	./$(TEST_EXEC)

.PHONY: all clean run
