# Copyright 2012, University of Freiburg,
# Chair of Algorithms and Data Structures.
# Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

include ./Makefile.inc

PROJECT = HashFinder
VPATH = algorithms

all: checkstyle compile test

compile: compile-main compile-test

compile-main: $(PROJECT)Main

compile-test: $(PROJECT)Test AlgorithmTest

%.o: %.cpp $(HEADERS)
	$(CXX) -c $< $(CXXFLAGS)
	
algorithms/%.o: %.cpp $(HEADERS)
	$(CXX) -c $< $(CXXFLAGS)

$(PROJECT)Main: $(PROJECT)Main.o HashFinder.o $(OBJECTS)
	$(CXX) -o $@ $^ $(MAINLIBS)

$(PROJECT)Test: $(PROJECT)Test.o HashFinder.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TESTLIBS)

checkstyle:
	python ../cpplint.py ./algorithms/*.h ./algorithms/*.cpp ../cpplint.py ./*.h ./*.cpp

test: $(PROJECT)Test AlgorithmTest
	@cd ./algorithms; make test;
	@echo === executing HashFinderTest ===
	./$(PROJECT)Test

AlgorithmTest:
	@cd ./algorithms; make compile;

clean:
	rm -f *Main *Test *.o
	@cd ./algorithms; make clean;
