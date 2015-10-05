// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.

#include <string>
#include "./HashAlgorithm.h"

HashAlgorithm::HashAlgorithm() {
  reset();
}

HashAlgorithm::~HashAlgorithm() {
}

void HashAlgorithm::reset() {
  finalized = false;
}

void HashAlgorithm::update(const std::string &s) {
}

void HashAlgorithm::update(const char *buf, size_t length) {
}

void HashAlgorithm::apply() {
}

HashAlgorithm& HashAlgorithm::finalize() {
  finalized = true;
  return *this;
}

std::string HashAlgorithm::hexdigest() const {
  return std::string("");
}
