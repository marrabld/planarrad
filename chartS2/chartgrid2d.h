
// chartgrid2d.h

#ifndef _JUDE_CHARTS2_CHARTGRID2D_H
#define _JUDE_CHARTS2_CHARTGRID2D_H

#include "drawS2/drawgroup2d.h"
#include "drawS2/line2d.h"
#include "chartS2/chartaxis2d.h"

using namespace jude::base;
using namespace jude::S2;
using namespace jude::drawS2;

namespace jude {
namespace chartS2 {



class ChartGrid2D {

  DrawGroup2D top_draw_group;

  JColour grid_col;
 
public:

  ChartGrid2D();
  ~ChartGrid2D();

  const DrawGroup2D& topDrawGroup() const { return top_draw_group; }

  void setAxes(const ChartAxis2D& x_axis, const ChartAxis2D& y_axis);
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTGRID2D_H

