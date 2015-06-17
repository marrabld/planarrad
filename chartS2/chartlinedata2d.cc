
// chartlinedata2d.cc

#include "chartlinedata2d.h"

using namespace jude::chartS2;



ChartLineData2D::ChartLineData2D(bool display_as_line) :
  x_data(0),
  y_data(0),
  display_as_line(display_as_line),
  highlighted_point(-1) { }

