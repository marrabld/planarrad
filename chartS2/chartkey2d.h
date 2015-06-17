
// chartkey2d.h

#ifndef _JUDE_CHARTS2_CHARTKEY2D_H
#define _JUDE_CHARTS2_CHARTKEY2D_H

#include "drawS2/drawgroup2d.h"
#include "drawS2/line2d.h"
#include "drawS2/rectangle2d.h"
#include "base/jlist.h"

using namespace jude::drawS2;
using namespace jude::base;

namespace jude {
namespace chartS2 {

class Chart2D;



class ChartKey2D {

  struct key_item {
    DrawItem2D* label;
    Line2D* line;
    key_item(DrawItem2D* di, const JColour& c) : label(di), line(new Line2D(Vec2D(0,0),Vec2D(0,0),c)) { }
  };

  JList<key_item> item_list;
 
  DrawGroup2D top_draw_group;

  Line2D* line_top;
  Line2D* line_bottom;
  Line2D* line_left;
  Line2D* line_right;

  Rectangle2D* back_rect;

  int left_margin;
  int right_margin;
  int top_margin;
  int bottom_margin;
  int line_length;
  int line_item_spacing;
  int item_x_spacing;
  int item_y_spacing;

public:

  ChartKey2D();
  ~ChartKey2D() { }

  const DrawGroup2D& topDrawGroup() const { return top_draw_group; }

  void addItem(DrawItem2D* di, const JColour col);

  void arrangeDrawing(const Chart2D& chart);
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTKEY2D_H



