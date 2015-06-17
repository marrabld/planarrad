
// chartaxis2d.h

#ifndef _JUDE_CHARTS2_CHARTAXIS2D_H
#define _JUDE_CHARTS2_CHARTAXIS2D_H

#include "drawS2/drawgroup2d.h"
#include "drawS2/label2d.h"
#include "drawS2/line2d.h"
#include "chartS2/chartnumberitemgen2d.h"

using namespace jude::base;
using namespace jude::S2;
using namespace jude::drawS2;

namespace jude {
namespace chartS2 {



class ChartAxis2D {

public:

  typedef enum { XTop=0, XBottom=1, YLeft=2, YRight=3 } axis_type;

private:

  bool is_defined;
  axis_type ax_type;
 
  DrawGroup2D top_draw_group;
 
  DrawItem2D* label_item;

  ChartNumberGen2D* number_item_gen;

  DrawGroup2D numbers_group;
  DrawGroup2D ticks_group;
 
  Vec2D line_pos0;
  Vec2D line_pos1;
 
  Line2D line_item;

  Box2D bounding_box;

  double min_value;
  double max_value;
  bool reverse_axis;
 
  bool is_log;

  bool display_numbers;
  int numbers_cross_size;
 
    // tick subdivision
  double sub;
    // first subdivision to draw
  double sub_start;
    // last subdivision to draw
  double sub_end;
    // minimum subdivision allowed
  double min_sub;
    // maximum subdivision tick allowed
  double max_sub_count;
    // count of the number of current tick positions regardless of whether ticks are drawn - used for grid
  int tick_location_count;

  const ChartAxis2D* link_subdiv;

  // bool min_fixed;
  // bool max_fixed;

    // the margin between the label and the numbers
  int label_margin;
    // the margin between the numbers and the ticks
  int numbers_margin;
    // the length of the tick
  int tick_length;

  JString user_axis_number_format;
  JString axis_number_format;

  //double max_displayed;  // maximum value to display
  //double min_displayed;  // minimum value to display

    // text label
  //Label2D label;
  //int text_x, text_y;
 
  //LineSeq2D axis_line;
  /*
  double axis_position;
  bool axis_centred;

  DrawGroup2D numbers;
  int numbers_x;  // only for a y-axis
  int numbers_y;  // only for an x-axis
  int numbers_w;  // only for a y-axis

  //JFont numbers_font;
  JColour numbers_colour;
 
  bool auto_scale;

  double axis_pos;
  int major_axis;
  double sub, sub_start, sub_end;

  double min_sub;
  int max_sub_num;

  bool has_size;      // is true if x size for the graph is determined
  */

  JString getAxisLabelString(double v) const;
 
public:

  ChartAxis2D(axis_type ax_type);
  ~ChartAxis2D();

  bool isDefined() const { return is_defined; }
  bool isXAxis() const { return (ax_type==XTop || ax_type==XBottom); }
  bool isYAxis() const { return (ax_type==YLeft || ax_type==YRight); }

  bool isLog() const { return is_log; } 
  void setIsLog(bool f) { is_log = f; } 

  const Vec2D& linePos0() const { return line_pos0; }
  const Vec2D& linePos1() const { return line_pos1; }
 
  const DrawGroup2D& topDrawGroup() const { return top_draw_group; }

  void setLabelItem(DrawItem2D* li);

  void setNumberGen(ChartNumberGen2D* ng);

    // returns the size at 90 degrees to the axis
  int crossSize() const;

  void setBoundingBox(const Box2D& bbox);

    /*! Sets the limits of automatic tick generation to be a minimum of step \a min_s and maximum number of ticks \a max_s_count. */ 
  void setTickSubdivision(double min_s, double max_s_count) {
    min_sub = min_s;
    max_sub_count = max_s_count;
  }

    /*! \brief Sets the value the two extreme ends of the axis represents. */
  void setValueRange(double min_v, double max_v) {
    min_value = min_v;
    max_value = max_v;
    reverse_axis = (max_value < min_value);
    //min_fixed = true;
    //max_fixed = true;
  }

  void setLinkSubdivisions(const ChartAxis2D* ptr) { link_subdiv = ptr; }
  const ChartAxis2D* linkSubdivisions() const { return link_subdiv; }

    /*! \brief Returns the value the lower extreme end of the axis represents. */
  double minValue() const { return min_value; }
    /*! \brief Returns the value the upper extreme end of the axis represents. */
  double maxValue() const { return max_value; }

  void setDisplayNumbers(bool f) { display_numbers = f; }

  void calcSubAxisIntervalsY(int pixel_length);
  void calcSubAxisIntervalsX(int pixel_length);

  int valueToPixelPosition(double v) const;

  void setLabelMargin(int t) { label_margin = t; }
  void setNumbersMargin(int t) { numbers_margin = t; }
  void setTickLength(int t) { tick_length = t; }

  void setAxisNumberFormat(const JString& s) { user_axis_number_format = s; }

  //void setAxisDisplayMin(double xmin) { sub_start = xmin; min_fixed = true; }
  //void setAxisDisplayMax(double xmax) { sub_end = xmax; max_fixed = true; }

  //double axisDisplayMin() const { return sub_start; }
  //double axisDisplayMax() const { return sub_end; }

  int tickLocationCount() const { return tick_location_count; }
  double tickLocation(int i) const { return (!reverse_axis) ? sub_start + i * sub : sub_start - i * sub; }
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTAXIS2D_H

