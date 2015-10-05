// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>

#ifndef PROJEKT_HASHFINDER_H_
#define PROJEKT_HASHFINDER_H_
#define HASHFINDER_VERSION "1.0"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

// Class for reading a list of words from a file
// or generating combinations out of a range of characters
// which then are MD5 or SHA1 hashed and compared against
// a hash which we want to find a collision for.
class HashFinder {
 public:
  // Constructor
  HashFinder();

  // Destructor
  ~HashFinder();

  // Set the default values, used by parseCommandLineArguments
  void reset();

  // Parse the command line arguments. The hash is specified as a non-option
  // argument and cannot be omitted. Apart from that, the following options
  // should be supported:
  // --hash-algorithm, -h : specifies whether SHA-1 or MD5 should be used
  // --input-file, -i  : read from a dictionary file.
  // --min-length, -a  : minimal length of the generated combinations
  // --max-length, -z  : maximum length of the generated combinations
  // --characters, -c  : characters which can be used to generate combinations
  // Defaults will be:
  // --hash-algorithm=md5
  // --min-length=8
  // --max-length=8
  // --characters=abcdefghijklmnopqrstuvwxyz0123456789
  void parseCommandLineArguments(int argc, char** argv);
  FRIEND_TEST(HashFinderTest, parseCommandLineArguments);

  // Read words from a dictionary.
  bool readDictionary();
  FRIEND_TEST(HashFinderTest, readDictionary);

  // Do the actual work, that is, create an MD5 or SHA1 hash
  // of a new word (if no dictionary is used, generate one)
  // Do all of this until a hash collision is found.
  void process(const unsigned threadnumber, const unsigned nThreads);
  FRIEND_TEST(HashFinderTest, process);

  // Print configuration info.
  void printConfiguration() const;
 private:
  // Print usage info and exit.
  void printUsageAndExit() const;

  // The hash string we will be searching for.
  const char* _hashToFind;

  // If we are not searching for an MD5 collision we want to try SHA1.
  bool _md5;

  // Save the allowed characters into the following string.
  // If empty, we will try words from the dictionary.
  const char* _allowedCharacters;

  // Specify the length of the word to search for.
  int _minLength;
  int _maxLength;

  // The filename of the dictionary to use.
  const char* _inputFileName;

  // The words from the dictionary (if specified).
  vector<string> _dictionary;

  // This variable is filled if a collision is found.
  // The threads should be canceled at this time.
  char* _collision;
};

#endif  // PROJEKT_HASHFINDER_H_
