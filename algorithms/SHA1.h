// Copyright 2012, 100% Public Domain :-)
// Authors: Steve Reid <steve@edmweb.com> (Original C Code)
// Authors: Bruce Guenter <bruce@untroubled.org> (Changes to fit into bglibs)
// Authors: Volker Grabsch <vog@notjusthosting.com> (Transl. to simpler C++)
/*
===============================================================================
This class has been modified to fit the needs of this project.
Jerome Meinke <meinkej@informatik.uni-freiburg.de>
Programmieren in C++ SS-2012
===============================================================================
*/


#ifndef PROJEKT_ALGORITHMS_SHA1_H_
#define PROJEKT_ALGORITHMS_SHA1_H_

#include <string>
#include "./HashAlgorithm.h"

class SHA1: public HashAlgorithm {
 public:
  SHA1();
  explicit SHA1(const std::string& text);
  void update(const std::string &s);
  void update(std::istream *is);
  void reset();
  SHA1& finalize();
  std::string hexdigest() const;

 private:
  bool finalized;

  /* number of 32bit integers per SHA1 digest */
  static const unsigned int DIGEST_INTS = 5;
  /* number of 32bit integers per SHA1 block */
  static const unsigned int BLOCK_INTS = 16;
  static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;

  uint32_t digest[DIGEST_INTS];
  std::string buffer;
  uint64_t transforms;

  void apply(uint32_t block[BLOCK_BYTES]);

  static void buffer_to_block(const std::string &buffer,
    uint32_t block[BLOCK_BYTES]);
  static void read(std::istream *is, std::string *s, const int max);
};

#endif  // PROJEKT_ALGORITHMS_SHA1_H_
