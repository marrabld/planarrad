
// jstring_testing.h

#ifndef _JUDE_BASE_JSTRING_TESTING_H
#define _JUDE_BASE_JSTRING_TESTING_H

#include "base/jtestcase.h"

namespace jude {
namespace base {

class JString;



class JString_Testing : public JTestCase {

  void textToInt(const JString& s, int n, int& tc, int& fc);
  void textToIntSegment(const JString& s, int start_pos, int end_pos, int n, int& tc, int& fc);

  void textToDouble(const JString& s, double v, int& tc, int& fc);
  void textToDoubleSegment(const JString& s, int start_pos, int end_pos, double v, int& tc, int& fc);

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::base

#endif // _JUDE_BASE_JSRING_TESTING_H

