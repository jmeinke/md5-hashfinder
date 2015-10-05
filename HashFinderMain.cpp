// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Jerome Meinke <meinkej@informatik.uni-freiburg.de>.


#include <thread>
#include <sys/sysinfo.h>
#include "./HashFinder.h"

unsigned possibleThreadCount() {
  #if defined(PTW32_VERSION) || defined(__hpux)
  return pthread_num_processors_np();
  #elif defined(__APPLE__) || defined(__FreeBSD__)
  int count;
  size_t size = sizeof(count);
  return sysctlbyname("hw.ncpu", &count, &size, NULL, 0) ? 0 : count;
  #elif defined(BOOST_HAS_UNISTD_H) && defined(_SC_NPROCESSORS_ONLN)
  int const count = sysconf(_SC_NPROCESSORS_ONLN);
  return (count > 0) ? count : 1;
  #elif defined(_GNU_SOURCE)
  return get_nprocs();
  #else
  return 1;
  #endif
}

// Main function.
int main(int argc, char** argv) {
  HashFinder hashfinder;
  hashfinder.parseCommandLineArguments(argc, argv);
  // we must read the dictionary in to a vector,
  // before being able to distribute the work to the different threads
  if (!hashfinder.readDictionary()) {
    printf("[Main] Error reading the dictionary file.\n");
    return 1;
  }

  hashfinder.printConfiguration();

  static const unsigned kThreadCount = possibleThreadCount();
  // static const unsigned kThreadCount = 1;
  std::thread t[kThreadCount];
  std::cout << "[Main] I will start " << kThreadCount << " threads now.\n";

  // launch as many threads as possible, start with threadnumber 1
  for (unsigned i = 1; i <= kThreadCount; ++i) {
    // let's make these threads execute the processing...
    t[i-1] = std::thread(&HashFinder::process, std::ref(hashfinder), i,
      kThreadCount);
  }

  // join the threads with the main thread
  for (unsigned i = 0; i < kThreadCount; ++i) {
    t[i].join();
  }
  std::cout << "[Main] Regular shutdown.\n";
  std::cout << "[Main] Thank you for using this program!\n";
  return 0;
}
