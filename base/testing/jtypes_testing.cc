
#include "jtypes_testing.h"
#include "base/jtypes.h"
#include "base/jlog.h"
#include "base/jstring.h"

using namespace jude::base;



// [private]

template <typename T>
bool JTypes_Testing::testSize(const JString& name, int exp, int& tc, int& fc) {

  int t = sizeof(T);
  jlog::ls << "Size of " << name << " is " << exp << " ... ";
  if (t == exp) { 
    jlog::ls << " Pass\n";
  } else {
    jlog::ls << " (actually " << t << ") Fail\n";
    fc++;
  }
  tc++;
  return (t == exp);
}



void JTypes_Testing::testLimits(int& tc, int& fc) {

  int32 min_int32 = INT32_MIN;
  int32 max_int32 = INT32_MAX;
  displaySuccess("int32 INT32_MIN representation", (min_int32 == INT32_MIN), tc, fc);
  displaySuccess("int32 INT32_MAX representation", (max_int32 == INT32_MAX), tc, fc);
  displaySuccess("int32 increment wrapping", (++max_int32 == INT32_MIN), tc, fc);
  displaySuccess("int32 decrement wrapping", (--min_int32 == INT32_MAX), tc, fc);

  int64 min_int64 = INT64_MIN;
  int64 max_int64 = INT64_MAX;
  displaySuccess("int64 INT64_MIN representation", (min_int64 == INT64_MIN), tc, fc);
  displaySuccess("int64 INT64_MAX representation", (max_int64 == INT64_MAX), tc, fc);
  displaySuccess("int64 increment wrapping", (++max_int64 == INT64_MIN), tc, fc);
  displaySuccess("int64 decrement wrapping", (--min_int64 == INT64_MAX), tc, fc);

  uns64 min_uns64 = UNS64_MIN;
  uns64 max_uns64 = UNS64_MAX;
  displaySuccess("uns64 UNS64_MIN representation", (min_uns64 == UNS64_MIN), tc, fc);
  displaySuccess("uns64 UNS64_MAX representation", (max_uns64 == UNS64_MAX), tc, fc);
  displaySuccess("uns64 increment wrapping", (++max_uns64 == UNS64_MIN), tc, fc);
  displaySuccess("uns64 decrement wrapping", (--min_uns64 == UNS64_MAX), tc, fc);

  uns32 min_uns32 = UNS32_MIN;
  uns32 max_uns32 = UNS32_MAX;
  displaySuccess("uns32 UNS32_MIN representation", (min_uns32 == UNS32_MIN), tc, fc);
  displaySuccess("uns32 UNS32_MAX representation", (max_uns32 == UNS32_MAX), tc, fc);
  displaySuccess("uns32 increment wrapping", (++max_uns32 == UNS32_MIN), tc, fc);
  displaySuccess("uns32 decrement wrapping", (--min_uns32 == UNS32_MAX), tc, fc);

}



void JTypes_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  testSize<int8>("int8", 1, tc, fc);
  testSize<int16>("int16", 2, tc, fc);
  testSize<int32>("int32", 4, tc, fc);
  testSize<int64>("int64", 8, tc, fc);
 
  testSize<uns8>("uns8", 1, tc, fc);
  testSize<uns16>("uns16", 2, tc, fc);
  testSize<uns32>("uns32", 4, tc, fc);
  testSize<uns64>("uns64", 8, tc, fc);

  testSize<float32>("float32", 4, tc, fc);
  testSize<double64>("double64", 8, tc, fc);

  if (!testSize<doubleLONG>("doubleLONG", 16, tc, fc)) {
    jlog::ls << "In the C standard long doubles can be as short as double, but something larger would be desirable.\n";
    tc--; 
    fc--;
    if (!testSize<doubleLONG>("doubleLONG", 12, tc, fc)) {
      jlog::ls << "This is being counted as an error but will almost certainly have no effect on accuracy.\n";
      jlog::ls << "In fact it will certainly have no effect on accuracy for anything in the open-source release\n";
      jlog::ls << "since doubleLONG is not even used in any of that code. So dont worry about it.\n";
    }
  }

  testLimits(tc, fc);

  jlog::ls << "JTypes testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  
  total_count+= tc;
  fail_count += fc;
}

