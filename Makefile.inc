# Copyright 2012, University of Freiburg,
# Chair of Algorithms and Data Structures.
# Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

CXX = g++
#CXX = g++ -pg
#CXXFLAGS = -Wall -g -std=c++0x
CXXFLAGS = -Wall -std=c++0x -O3
MAINLIBS = -lpthread
TESTLIBS = -lgtest -lgtest_main -lpthread
HEADERS = $(wildcard *.h)
OBJECTS = HashAlgorithm.o SHA1.o MD5.o
