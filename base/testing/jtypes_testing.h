
// jtypes_testing.h

#ifndef _JUDE_BASE_JTYPES_TESTING_H
#define _JUDE_BASE_JTYPES_TESTING_H

#include "base/jtestcase.h"

namespace jude {
namespace base {

class JString;



class JTypes_Testing : public JTestCase {

  template <typename T>
  bool testSize(const JString& name, int exp, int& tc, int& fc);

  void testLimits(int& tc, int& fc);

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::base

#endif // _JUDE_BASE_JTYPES_TESTING_H

