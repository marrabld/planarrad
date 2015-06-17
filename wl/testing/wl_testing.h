
// wl_testing.h

#ifndef _JUDE_WL_WL_TESTING
#define _JUDE_WL_WL_TESTING

#include "base/jtestcase.h"

using namespace jude::base;

namespace jude {
namespace wl {



class WL_Testing : public JTestCase {
 
public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::drawS2

#endif // _JUDE_WL_WL_TESTING

