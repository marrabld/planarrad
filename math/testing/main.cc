
#include "base/jlog.h"
#include "rand_testing.h"
#include "config.h"



using namespace jude::math;



int main() {

  JLog::init();

  Rand_Testing rand_test;

  int total_count = 0;
  int fail_count = 0;

  //rand_test.runTests(total_count, fail_count);


  return (fail_count == 0) ? 0 : 1;
}

