
// drawing2d_testing.h

#ifndef _JUDE_DRAWS2_DRAWING2D_TESTING_H
#define _JUDE_DRAWS2_DRAWING2D_TESTING_H

#include "base/jtestcase.h"
#include "base/jbuffer.h"
#include "drawS2/drawing2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



class Drawing2D_Testing : public JTestCase {

  void makeTriangle(Drawing2D& drawing);
  void drawDrawing(JBuffer& buff, int x_size, int y_size, const Drawing2D& drawing);
 
public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_DRAWING2D_TESTING_H

