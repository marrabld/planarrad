
// chartlabel2d.h

#ifndef _JUDE_CHARTS2_CHARTLABEL2D_H
#define _JUDE_CHARTS2_CHARTLABEL2D_H

#include "drawS2/drawitem2d.h"

using namespace jude::drawS2;

namespace jude {
namespace chartS2 {

class Chart2D;



class ChartLabel2D {

  DrawItem2D* label_item;

  typedef enum { PlotAreaCenter, Absolute } alignment;

  alignment v_align;
  alignment h_align;
 
public:

  ChartLabel2D(DrawItem2D* di) : 
    label_item(di),
    v_align(PlotAreaCenter),
    h_align(PlotAreaCenter) { }

  ~ChartLabel2D() { delete label_item; }

  const DrawItem2D& drawItem() { return *label_item; }

  void arrangeDrawing(const Chart2D& chart);
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTLABEL2D_H



