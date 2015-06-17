
#include "base/jtestcase.h"
#include "base/jlog.h"

using namespace jude::base;


/*! Displays \a message, Pass or Fail dependent on \a s and increments \a total_count and \a fail_count if \a s is \c false. */

void JTestCase::displaySuccess(const JString& message, bool s, int& total_count, int& fail_count) const {
  jlog::ls << message << " ... " << (s ? "Pass\n" : "Fail\n");
  if (!s) fail_count++;
  total_count++;
}

