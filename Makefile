CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
LDFLAGS  =

LIB_SRC  = lib/liba.cpp
LIB_OBJ  = lib/liba.o
LIB_INC  = -Ilib

LABS     = lab1 lab2 lab3 lab5

.PHONY: all clean $(LABS)

all: $(LABS)

$(LIB_OBJ): $(LIB_SRC) lib/liba.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

lab1: lab1/main
lab2: lab2/main
lab3: lab3/main
lab5: lab5/main

lab1/main: lab1/main.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(LDFLAGS)

lab2/main: lab2/main.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(LDFLAGS)

lab3/main: lab3/main.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(LDFLAGS)
lab5/main: lab5/main.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(LDFLAGS)


GTEST_LIBS = -lgtest -lgtest_main -pthread

lab1/test: lab1/test.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(GTEST_LIBS)

lab2/test: lab2/test.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(GTEST_LIBS)

lab3/test: lab3/test.cpp $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(LIB_INC) $^ -o $@ $(GTEST_LIBS)

test: lab1/test lab2/test lab3/test
	
test1: lab1/test
	./lab1/test

test2: lab2/test
	./lab2/test

test3: lab3/test
	./lab3/test

	
clean:
	rm -f lib/*.o lab1/main lab2/main