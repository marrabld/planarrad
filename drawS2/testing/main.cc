
#include "base/jlog.h"
#include "drawing2d_testing.h"

using namespace jude::drawS2;



int main() {

  JLog::init();

  Drawing2D_Testing drawing2d_test;

  int total_count = 0;
  int fail_count = 0;

  drawing2d_test.runTests(total_count, fail_count);

  jlog::ls << "jude::drawS2  passed a total of " << (total_count-fail_count) << " of " << total_count << " tests.\n";

  return (fail_count == 0) ? 0 : 1;
}

