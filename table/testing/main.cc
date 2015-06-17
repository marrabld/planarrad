
#include "base/jlog.h"
#include "table_testing.h"

using namespace jude::table;



int main() {

  JLog::init();

  Table_Testing table_test;

  int total_count = 0;
  int fail_count = 0;

  table_test.runTests(total_count, fail_count);

  return (fail_count == 0) ? 0 : 1;
}

