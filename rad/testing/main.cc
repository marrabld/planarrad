
#include "base/jlog.h"
#include "bandspec_testing.h"

using namespace jude::base;



int main() {

  JLog::init();

  BandSpec_Testing bandspec_test;
 
  int total_count = 0;
  int fail_count = 0;

  bandspec_test.runTests(total_count, fail_count);

  jlog::ls << "jude::rad passed a total of " << (total_count-fail_count) << " of " << total_count << " tests.\n";

  return (fail_count == 0) ? 0 : 1;
}

