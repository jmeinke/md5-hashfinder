// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

#include <gtest/gtest.h>
#include <string>
#include "./MD5.h"
#include "./SHA1.h"

// Test generating MD5-hashes
TEST(MD5, TestingHashIsCorrect) {
  MD5 * test = new MD5("Message-Digest Algorithm 5 (MD5)");
  ASSERT_STREQ(test->hexdigest().c_str(),
      "291c6183ebadebd1a87aa6074176a837");
  test->reset();

  std::string mystr("Ronald L. Rivest 1991");
  test->update(mystr.c_str(), mystr.size());
  test->finalize();
  ASSERT_STREQ(test->hexdigest().c_str(),
      "d970bc029b1e5ae3f8e2e1aab13dbd06");
  delete test;
}

// Test generating SHA1-hashes
TEST(SHA1, TestingHashIsCorrect) {
  SHA1 * test = new SHA1("secure hash algorithm (SHA-1)");
  ASSERT_STREQ(test->hexdigest().c_str(),
      "1df760022cdb097eaaf6ab4a66f87a8fa298750e");
  test->reset();

  test->update(std::string("United States National Security Agency 1995"));
  test->finalize();
  ASSERT_STREQ(test->hexdigest().c_str(),
      "3065cba73613f3ad4209a3bb0c08f5dbf35fba24");
  delete test;
}
