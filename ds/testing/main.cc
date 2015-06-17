
#include "base/jlog.h"
#include "ds_testing.h"

using namespace jude::ds;



int main() {

  JLog::init();

  DS_Testing ds_test;

  int total_count = 0;
  int fail_count = 0;

  ds_test.runTests(total_count, fail_count);

  return (fail_count == 0) ? 0 : 1;
}

