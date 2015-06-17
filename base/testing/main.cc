
#include "base/jlog.h"
#include "jtypes_testing.h"
#include "jstring_testing.h"
#include "jimagesave_testing.h"

using namespace jude::base;



int main() {

  printf("test\n");

  JLog::init();

  JTypes_Testing jtypes_test;
  JString_Testing jstring_test;
  JImageSave_Testing jimagesave_test;

  int total_count = 0;
  int fail_count = 0;

  jtypes_test.runTests(total_count, fail_count);
  jstring_test.runTests(total_count, fail_count);
  jimagesave_test.runTests(total_count, fail_count);

  jlog::ls << "jude::base passed a total of " << (total_count-fail_count) << " of " << total_count << " tests.\n";

  return (fail_count == 0) ? 0 : 1;
}

