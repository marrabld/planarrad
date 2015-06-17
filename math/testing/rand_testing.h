
// rand_testing.h

#ifndef _JUDE_MATH_RAND_TESTING_H
#define _JUDE_MATH_RAND_TESTING_H

#include "base/jtestcase.h"
#include "base/jstring.h"
#include "math/rangen.h"
#include "math/rand.h"

using namespace jude::base;

namespace jude {
namespace math {



class Rand_Testing : public JTestCase {

  class Test_uniform_dbn : public RanGen {
  public:
    void initSeed(int32 i) { }
    double generateDouble() { return uniform_dbn(0,1); }
  };

  class Test_uniform_dbn_cstd : public RanGen {
  public:
    void initSeed(int32 i) { }
    double generateDouble() { return uniform_dbn_cstd(0,1); }
  };

  template <typename RG>
  bool uniformTest(const JString& name);

  bool normalTest(const JString& func, double mean, double sd, double req_acc);

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::math

#endif // _JUDE_MATH_RAND_TESTING_H

