
// jtestcase.h

#ifndef _JUDE_BASE_TESTCASE_H
#define _JUDE_BASE_TESTCASE_H

#include "base/jstring.h"

namespace jude {
namespace base {



/*! \brief Interface for code testing routines. */

class JTestCase {

public:

  virtual ~JTestCase() { }

  virtual void runTests(int& total_count, int& fail_count) = 0;

  void displaySuccess(const JString& message, bool s, int& total_count, int& fail_count) const;
};



} } // namespace jude::base

#endif // _JUDE_BASE_TESTCASE_H

