
// ds_testing.h

#ifndef _JUDE_DS_DS_TESTING_H
#define _JUDE_DS_DS_TESTING_H

#include "base/jtestcase.h"
#include "base/jstring.h"

using namespace jude::base;

namespace jude {
namespace ds {



class DS_Testing : public JTestCase {

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::ds

#endif // _JUDE_DS_DS_TESTING_H

