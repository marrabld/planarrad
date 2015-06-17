
// bandspec_testing.h

#ifndef _JUDE_RAD_BANDSPEC_TESTING_H
#define _JUDE_RAD_BANDSPEC_TESTING_H

#include "base/jtestcase.h"
#include "rad/bandspec.h"
#include "rad/sbanddata.h"

using namespace jude::rad;

namespace jude {
namespace base {

class JString;



class BandSpec_Testing : public JTestCase {

  void writePlotFile(const JString& fp, const SBandData& sd1, const BandSpec& bs1, const SBandData& sd2, const BandSpec& bs2);

public:

  void runTests(int& total_count, int& fail_count);
  
};



} } // namespace jude::base

#endif // _JUDE_RAD_BANDSPEC_TESTING_H

