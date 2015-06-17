
// chartlinedata2d.h

#ifndef _JUDE_CHARTS2_CHARTLINEDATA2D_H
#define _JUDE_CHARTS2_CHARTLINEDATA2D_H

#include "math/ublas.h"
#include "base/jcolour.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace chartS2 {



class ChartLineData2D {

  JColour col;

  vector<double> x_data;
  vector<double> y_data;

  bool display_as_line;

  int highlighted_point;

public:

  ChartLineData2D(bool display_as_line=true);

  const JColour& colour() const { return col; }
  void setColour(const JColour& c) { col = c; }

  void clearData() {
    x_data.resize(0);
    y_data.resize(0);
  }

  void setData(const vector<double>& xd, const vector<double>& yd) { 
    x_data = xd; 
    y_data = yd; 
  }
 
  const vector<double>& xData() const { return x_data; }
  const vector<double>& yData() const { return y_data; }

  bool displayAsLine() const { return display_as_line; }

  int highlightedPoint() const { return highlighted_point; }
  void setHighlightedPoint(int p) { highlighted_point = p; }
  void clearHighlightedPoint() { highlighted_point = -1; }
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTLINEDATA2D_H


