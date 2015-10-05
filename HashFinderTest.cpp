// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "./HashFinder.h"

// Test parsing the command line arguments
TEST(HashFinderTest, parseCommandLineArguments) {
  HashFinder hashfinder;

  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  // Call without options and without argument.
  {
    int argc = 1;
    char* argv[2] = { const_cast<char*>("egal") };
    ASSERT_DEATH(hashfinder.parseCommandLineArguments(argc, argv), "Usage:.*");
  }

  // Regular call with input-file option and a valid MD5-Hash
  {
    int argc = 3;
    char* argv[3] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--input-file=notExisting.txt"),
      const_cast<char*>("35e5d160921d131d9114f1b4ee5f9d55")
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_TRUE(hashfinder._md5);
    ASSERT_STREQ("notExisting.txt", hashfinder._inputFileName);
    ASSERT_STREQ("abcdefghijklmnopqrstuvwxyz0123456789",
        hashfinder._allowedCharacters);
    ASSERT_STREQ("35e5d160921d131d9114f1b4ee5f9d55", hashfinder._hashToFind);
  }

  // Regular call with input-file option and a valid SHA-1-Hash
  {
    int argc = 4;
    char* argv[4] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--input-file=notExisting.txt"),
      const_cast<char*>("--hash-algo=sha1"),
      const_cast<char*>("586b64caacfbdfd67d2a8d323510ed5b72a61e0b")
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_FALSE(hashfinder._md5);
    ASSERT_STREQ("notExisting.txt", hashfinder._inputFileName);
    ASSERT_STREQ("abcdefghijklmnopqrstuvwxyz0123456789",
        hashfinder._allowedCharacters);
    ASSERT_STREQ("586b64caacfbdfd67d2a8d323510ed5b72a61e0b",
        hashfinder._hashToFind);
  }

  // Regular call with algorithm option, characters and SHA-1
  {
    int argc = 4;
    char* argv[4] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--hash-algo=sha1"),
      const_cast<char*>("--characters=sha1"),
      const_cast<char*>("586b64caacfbdfd67d2a8d323510ed5b72a61e0b")
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_FALSE(hashfinder._md5);
    ASSERT_STREQ("sha1", hashfinder._allowedCharacters);
    ASSERT_STREQ(NULL, hashfinder._inputFileName);
    ASSERT_STREQ("586b64caacfbdfd67d2a8d323510ed5b72a61e0b",
        hashfinder._hashToFind);
  }
}

// Test loading a dictionary-file
TEST(HashFinderTest, readDictionary) {
  HashFinder hashfinder;

  // first write the test file
  const char* testFileName = "exampleDictionary.txt";
  std::ofstream myfile(testFileName);
  if (myfile.is_open()) {
    myfile << "Dauerschlaf\n";
    myfile << "Radschaufel\n";
    myfile << "Schaufelrad";
    myfile.close();
  } else {
    printf("Unable to open exampleDictionary for writing.\n");
    FAIL();
  }

  // Now try to load the test file
  hashfinder._inputFileName = "exampleDictionary.txt";
  ASSERT_TRUE(hashfinder.readDictionary());
  remove(testFileName);

  // Check if we have the same amount of entries as lines
  ASSERT_EQ(hashfinder._dictionary.size(), 3);

  // Check the first word of the dictionary
  ASSERT_STREQ("Dauerschlaf", hashfinder._dictionary[0].c_str());
}

// Test find MD5 and SHA1 in a dictionary file or in combinations
TEST(HashFinderTest, process) {
  HashFinder hashfinder;

  // first write the test file
  const char* testFileName = "exampleDictionary.txt";
  std::ofstream myfile(testFileName);
  if (myfile.is_open()) {
    myfile << "Dauerschlaf\n";
    myfile << "Radschaufel\n";
    myfile << "Schaufelrad";
    myfile.close();
  } else {
    printf("Unable to open exampleDictionary for writing.\n");
    FAIL();
  }

  {
    printf("Testing MD5 + combinations...\n");
    int argc = 6;
    char* argv[6] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--hash-algo=md5"),
      const_cast<char*>("--min-length=4"),
      const_cast<char*>("--max-length=4"),
      const_cast<char*>("--characters=fhlvz79"),
      const_cast<char*>("27b411b92e1ffa0250a1765b6fd152f2")  // hvl7
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_TRUE(hashfinder._md5);
    ASSERT_STREQ("fhlvz79",
      hashfinder._allowedCharacters);
    ASSERT_STREQ("27b411b92e1ffa0250a1765b6fd152f2",
        hashfinder._hashToFind);

    // Now start processing and let's check if the _collision
    // variable has been filled with the right word
    hashfinder.process(1, 1);
    ASSERT_STREQ("hvl7", hashfinder._collision);
  }

  {
    printf("Testing MD5 + dictionary...\n");
    int argc = 4;
    char* argv[4] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--input-file=exampleDictionary.txt"),
      const_cast<char*>("--hash-algo=md5"),
      const_cast<char*>("af26c4895ba73daaf0c05c7700d13041")
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_TRUE(hashfinder._md5);
    ASSERT_STREQ("exampleDictionary.txt", hashfinder._inputFileName);
    ASSERT_STREQ("af26c4895ba73daaf0c05c7700d13041",
        hashfinder._hashToFind);

    // Now load the dictionary file
    ASSERT_TRUE(hashfinder.readDictionary());

    // Now start processing and let's check if the _collision
    // variable has been filled with the right word
    hashfinder.process(1, 1);
    ASSERT_STREQ("Radschaufel", hashfinder._collision);
  }

  {
    printf("Testing SHA-1 + combinations...\n");
    int argc = 6;
    char* argv[6] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--hash-algo=sha-1"),
      const_cast<char*>("--min-length=4"),
      const_cast<char*>("--max-length=4"),
      const_cast<char*>("--characters=fhlvz79"),
      const_cast<char*>("754174b26b4e39f5089da8a3fda7f752832b82a0")  // hvl7
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_FALSE(hashfinder._md5);
    ASSERT_STREQ("fhlvz79",
      hashfinder._allowedCharacters);
    ASSERT_STREQ("754174b26b4e39f5089da8a3fda7f752832b82a0",
        hashfinder._hashToFind);

    // Now start processing and let's check if the _collision
    // variable has been filled with the right word
    hashfinder.process(1, 1);
    ASSERT_STREQ("hvl7", hashfinder._collision);
  }

  {
    printf("Testing SHA-1 + dictionary...\n");
    int argc = 4;
    char* argv[4] = {
      const_cast<char*>("HashFinderMain"),
      const_cast<char*>("--input-file=exampleDictionary.txt"),
      const_cast<char*>("--hash-algo=sha1"),
      const_cast<char*>("16115caaf988bd19f019d2812fe3dc0ba3a68e52")
    };
    hashfinder.parseCommandLineArguments(argc, argv);
    ASSERT_FALSE(hashfinder._md5);
    ASSERT_STREQ("exampleDictionary.txt", hashfinder._inputFileName);
    ASSERT_STREQ("16115caaf988bd19f019d2812fe3dc0ba3a68e52",
        hashfinder._hashToFind);

    // Now load the dictionary file
    ASSERT_TRUE(hashfinder.readDictionary());

    // Now start processing and let's check if the _collision
    // variable has been filled with the right word
    hashfinder.process(1, 1);
    ASSERT_STREQ("Radschaufel", hashfinder._collision);
  }
  remove(testFileName);
}
