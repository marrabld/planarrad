
// chart2d_testing.h

#ifndef _JUDE_CHARTS2_CHART2D_TESTING_H
#define _JUDE_CHARTS2_CHART2D_TESTING_H

#include "base/jtestcase.h"
#include "base/jbuffer.h"
#include "chartS2/chart2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace chartS2 {



class Chart2D_Testing : public JTestCase {

    void makeChart1(Chart2D& chart);

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHART2D_TESTING_H

