// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <math.h>

// This define is needed to make the code portable
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/time.h>  // for time measurement
#include <iostream>
#include <fstream>
#include <string>
#include "./algorithms/MD5.h"
#include "./algorithms/SHA1.h"
#include "./HashFinder.h"

// Constructor without arguments
HashFinder::HashFinder() {
  reset();
}

// Set the default values
void HashFinder::reset() {
  _collision = NULL;
  _inputFileName = NULL;
  _hashToFind = NULL;
  _minLength = 8;
  _maxLength = 8;
  _allowedCharacters = "abcdefghijklmnopqrstuvwxyz0123456789";
  _md5 = true;
  _dictionary.clear();
}

// Deconstructor
HashFinder::~HashFinder() {
  delete[] _collision;
  _inputFileName = NULL;
  _hashToFind = NULL;
  _allowedCharacters = NULL;
  _dictionary.clear();
}

void HashFinder::parseCommandLineArguments(int argc, char** argv) {
  reset();
  struct option options[] = {
    { "input-file", 1, NULL, 'i' },
    { "min-length", 1, NULL, 'a' },
    { "max-length", 1, NULL, 'z' },
    { "characters", 1, NULL, 'c' },
    { "hash-algo", 1, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };
  optind = 1;
  while (true) {
    char c = getopt_long(argc, argv, "i:a:z:c:h:", options, NULL);
    if (c == -1) break;
    switch (c) {
      case 'i':
        _inputFileName = optarg;
        break;
      case 'a':
        if (_inputFileName != NULL) {
          fprintf(stderr, "<min-length> will be ignored, using dictionary.\n");
        } else {
          _minLength = atoi(optarg);
          if ( _minLength <= 0 ) {
            fprintf(stderr, "<min-length> must be greater than 0.\n");
            exit(1);
          }
        }
        break;
      case 'z':
        if (_inputFileName != NULL) {
          fprintf(stderr, "<max-length> will be ignored, using dictionary.\n");
        } else {
          _maxLength = atoi(optarg);
          if ( _maxLength <= 0 ) {
            fprintf(stderr, "<max-length> must be greater than 0.\n");
            exit(1);
          }
        }
        break;
      case 'c':
        if (_inputFileName != NULL) {
          fprintf(stderr, "<characters> will be ignored, using dictionary.\n");
        } else {
          _allowedCharacters = optarg;
        }
        break;
      case 'h':
        char test1[] = "sha1";
        char test2[] = "sha-1";
        char * strToTest = new char[strlen(optarg)];
        snprintf(strToTest, sizeof(strToTest), "%s", optarg);
        if (strcmp(optarg, test1) == 0 || strcmp(optarg, test2) == 0)
          _md5 = false;
        delete[] strToTest;
        break;
    }
  }
  if (optind + 1 != argc) printUsageAndExit();
  _hashToFind = argv[optind];

  // verify min-length is not greater than max-length
  if (_minLength > _maxLength) _minLength = _maxLength;

  // verify length of the hash to find
  if (_md5) {
    if (strlen(_hashToFind) != 32) {
      fprintf(stderr, "<hashToFind> must be a hex string with length = 32.\n");
      exit(1);
    }
  } else {
    if (strlen(_hashToFind) != 40) {
      fprintf(stderr, "<hashToFind> must be a hex string with length = 40.\n");
      exit(1);
    }
  }
}

// Read the dictionary file into our vector
bool HashFinder::readDictionary() {
  if (_inputFileName == NULL) return true;

  // first empty the dictionary vector
  _dictionary.clear();

  std::string line;

  // open the dictionary and read line by line into our vector
  std::ifstream dictionaryFile(_inputFileName, std::ios_base::in);
  if (dictionaryFile.is_open()) {
    while ( dictionaryFile.good() ) {
      // read the file line by line
      getline(dictionaryFile, line);
      // save the line string into the _dictionary vector
      _dictionary.push_back(line);
    }
    // close the file handle
    dictionaryFile.close();
    return true;
  }
  return false;
}

// Print the usage and exit
void HashFinder::printUsageAndExit() const {
  fprintf(stderr,
          "Usage: ./HashFinderMain [options] <hashToFind>\n"
          "Options:\n"
          " -i, --input-file: read words from a dictionary file\n"
          " -a, --min-length: minimal length of the generated combinations\n"
          "                   Default: 8\n"
          " -z, --max-length: maximum length of the generated combinations\n"
          "                   Default: 8\n"
          " -c, --characters: chars used to generate combinations\n"
          "                   Default: abcdefghijklmnopqrstuvwxyz0123456789\n"
          " -h, --hash-algo : can either be sha-1 or md5\n"
          "                   Default: md5\n");
  exit(1);
}

// prints the configuration
void HashFinder::printConfiguration() const {
  printf("[Main] HashFinder version %s.\n", HASHFINDER_VERSION);
  printf("[Main] Hashing-Algorithm: %s.\n", _md5 ? "MD5" : "SHA-1");
  printf("[Main] Hash: %s.\n", _hashToFind);
  if (_inputFileName == NULL) {
    printf("[Main] Using combination attack:\n");
    if (_minLength != _maxLength) {
      printf("       - word length: %d - %d\n", _minLength, _maxLength);
    } else {
      printf("       - word length: %d\n", _maxLength);
    }
    printf("       - characters: %s\n", _allowedCharacters);
    uint64_t nCombinations = 0;
    for (int i = _minLength; i <= _maxLength; i++) {
      nCombinations += pow(strlen(_allowedCharacters), i);
    }
    printf("[Main] Combinations: %" PRIu64 "\n", nCombinations);
  } else {
    printf("[Main] Using dictionary attack: %zu words\n", _dictionary.size());
  }
}

void HashFinder::process(const unsigned threadnumber, const unsigned kThreads) {
  // when the thread starts print start message once
  printf("[Thread %d] Started...\n", threadnumber);

  // capture the start time
  struct timeval start_t, end_t;
  gettimeofday(&start_t, NULL);

  // how many combinations have been tried by this thread
  uint64_t nCombinationsTried = 0;
  // the work start at index 0, there we will add one,
  // if this is the first thread
  if (threadnumber == 1) nCombinationsTried++;

  // are we performing a dictionary attack
  if (_dictionary.size() > 0) {
    // this is the number of possible entries from the dictionary
    const uint64_t nEntries = _dictionary.size();
    // this is the start number of the entries this thread will compute
    uint64_t start = ((threadnumber - 1) * nEntries) / kThreads;
    // this is the stop number of combinations this thread will compute
    uint64_t stop = (threadnumber * nEntries) / kThreads;
    // this is the overhead, if we've got 2 threads and 3 combinations,
    // we need to put the third combination on one of those two threads
    // in this case we will put the overhead on the last thread
    if (threadnumber == kThreads) stop += nEntries % kThreads;

    // Now initialize the HashAlgorithm object
    HashAlgorithm * test;
    if (_md5) {
      test = new MD5();
    } else {
      test = new SHA1();
    }

    uint64_t k = start;
    for (; k < (stop - 1); ++k) {
      if (_collision != NULL) break;

      test->reset();
      if (_md5) {
        test->update(_dictionary.at(k).c_str(), _dictionary.at(k).size());
      } else {
        test->update(_dictionary.at(k));
      }
      test->finalize();

      if (strcmp(_hashToFind, test->hexdigest().c_str()) == 0) {
        _collision = new char[_dictionary.at(k).size()+1];
        snprintf(_collision, _dictionary.at(k).size()+1,
            _dictionary.at(k).c_str());
        printf("[Thread %d] Collision found => %s\n", threadnumber,
          _dictionary.at(k).c_str());
        break;
      }
    }
    nCombinationsTried += (k - start);
    delete test;
  } else {
    // divide the number of combinations for every thread for
    // the whole range of word lengths, ex. 5-6 or 3-8...
    for (int wlen = _minLength; wlen <= _maxLength; wlen++) {
      const unsigned kCharLength = wlen;
      char combination[kCharLength];
      uint64_t l;
      div_t x;

      // this is the number of possible combinations for this word length
      const uint64_t nCombinations = pow(strlen(_allowedCharacters),
          kCharLength);
      // this is the start number of the combinations this thread will compute
      uint64_t start = ((threadnumber - 1) * nCombinations) / kThreads;
      // this is the stop number of combinations this thread will compute
      uint64_t stop = (threadnumber * nCombinations) / kThreads;
      // this is the overhead, if we've got 2 threads and 3 combinations,
      // we need to put the third combination on one of those two threads
      // in this case we will put the overhead on the last thread
      if (threadnumber == kThreads) stop += nCombinations % kThreads;

      // Now initialize the HashAlgorithm object
      HashAlgorithm * test;
      if (_md5) {
        test = new MD5();
      } else {
        test = new SHA1();
      }

      uint64_t k = start;
      for (; k <= stop; ++k) {
        if (_collision != NULL) break;
        l = k;
        for (int i = (kCharLength-1); i >= 0; --i) {
          x = div(l, pow(strlen(_allowedCharacters), i));
          combination[i] = _allowedCharacters[x.quot];
          l = x.rem;
        }

        test->reset();
        if (_md5) {
          const char * temp = combination;
          test->update(temp, sizeof(combination));
        } else {
          test->update(std::string(combination, kCharLength));
        }
        test->finalize();

        if (strcmp(_hashToFind, test->hexdigest().c_str()) == 0) {
          _collision = new char[kCharLength + 1];
          snprintf(_collision, kCharLength + 1, combination);
          printf("[Thread %d] Collision found => %.*s\n", threadnumber,
              kCharLength, _collision);
          break;
        }
      }
      nCombinationsTried += (k - (start+1));
      delete test;
    }
  }
  gettimeofday(&end_t, NULL);
  uint64_t endtime = (end_t.tv_sec * (unsigned int)1e6 +   end_t.tv_usec);
  uint64_t starttime = (start_t.tv_sec * (unsigned int)1e6 + start_t.tv_usec);
  printf("[Thread %d] Tried %" PRIu64 " strings.\n", threadnumber,
            nCombinationsTried);
  printf("[Thread %d] Stopped after %" PRIu64 " microseconds.\n", threadnumber,
    (endtime - starttime));
}
