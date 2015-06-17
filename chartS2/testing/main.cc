
#include "base/jlog.h"
#include "chart2d_testing.h"

using namespace jude::chartS2;



int main() {

  JLog::init();

  Chart2D_Testing chart2d_test;

  int total_count = 0;
  int fail_count = 0;

  chart2d_test.runTests(total_count, fail_count);

  jlog::ls << "jude::chartS2  passed a total of " << (total_count-fail_count) << " of " << total_count << " tests.\n";

  return (fail_count == 0) ? 0 : 1;
}

