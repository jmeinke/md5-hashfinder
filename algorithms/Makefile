# Copyright 2012, University of Freiburg,
# Chair of Algorithms and Data Structures.
# Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

include ../Makefile.inc

all: compile test

compile: AlgorithmTest HashAlgorithm.o MD5.o SHA1.o

%.o: %.cpp $(HEADERS)
	$(CXX) -c $< $(CXXFLAGS)

%Test: %Test.o $(OBJECTS)
	$(CXX) -o $@ $^ $(TESTLIBS)

test: AlgorithmTest
	@echo === executing AlgorithmTest ===
	./AlgorithmTest

clean:
	rm -f *Test *.o
