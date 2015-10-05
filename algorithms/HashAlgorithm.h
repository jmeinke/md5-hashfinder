// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

#ifndef PROJEKT_ALGORITHMS_HASHALGORITHM_H_
#define PROJEKT_ALGORITHMS_HASHALGORITHM_H_

#include <cstdint>
#include <string>
#include <iostream>

class HashAlgorithm {
 public:
  HashAlgorithm();
  virtual ~HashAlgorithm();

  virtual void update(const std::string &s);
  virtual void update(const char *buf, size_t length);
  virtual void reset();
  virtual HashAlgorithm& finalize();
  virtual std::string hexdigest() const;

 protected:
  bool finalized;
  virtual void apply();
};

#endif  // PROJEKT_ALGORITHMS_HASHALGORITHM_H_
