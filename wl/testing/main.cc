
#include "base/jlog.h"
#include "wl_testing.h"

using namespace jude::wl;



int main() {

  JLog::init();

  WL_Testing wl_test;

  int total_count = 0;
  int fail_count = 0;

  wl_test.runTests(total_count, fail_count);

  jlog::ls << "jude::wl  passed a total of " << (total_count-fail_count) << " of " << total_count << " tests.\n";

  return (fail_count == 0) ? 0 : 1;
}

