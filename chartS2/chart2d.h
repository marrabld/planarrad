
// chart2d.h

#ifndef _JUDE_CHARTS2_CHART2D_H
#define _JUDE_CHARTS2_CHART2D_H

#include "base/jlist.h"
#include "drawS2/drawing2d.h"
#include "drawS2/trans2d.h"
#include "chartaxis2d.h"
#include "chartgrid2d.h"
#include "chartlinedata2d.h"
#include "chartlabel2d.h"
#include "chartkey2d.h"

using namespace jude::base;
using namespace jude::drawS2;
using namespace jude::S2;

namespace jude {
namespace chartS2 {



class Chart2D {

    // the background
  Drawing2D chart_paper;

  ChartAxis2D x_top_axis;
  ChartAxis2D x_bottom_axis;
  ChartAxis2D y_left_axis;
  ChartAxis2D y_right_axis;

  ChartGrid2D chart_grid;

  ChartKey2D* chart_key;

    // the basic translation for labels etc, is 1:1
  //Trans2D paper_trans;
    // translation for axes set 0
  Trans2D data0_trans;
    // translation for axes set 1, if used
  Trans2D data1_trans;

  DrawGroup2D under_lines_group;
  DrawGroup2D over_lines_group;

  DrawGroup2D data00_group;
  DrawGroup2D data01_group;
  DrawGroup2D data10_group;
  DrawGroup2D data11_group;

    // list of line data sets for axes sets
  JList<ChartLineData2D> data00_list;
  JList<ChartLineData2D> data01_list;
  JList<ChartLineData2D> data10_list;
  JList<ChartLineData2D> data11_list;

    // text labels or other things to put on the plot
  JList<ChartLabel2D> label_list;

    // outer margins at the edge of the figure
  int left_margin;
  int right_margin;
  int top_margin;
  int bottom_margin;

    // title object
  DrawItem2D* title_item;
    // the margin on the inside edge of the title
  int title_margin;

    // the current size and location of the plot area in pixels
    // note co-ord system is (0,0) at bottom left 
  Vec2D plot_size;
  Vec2D plot_origin;

  void prepareData(JList<ChartLineData2D>& data_list, DrawGroup2D& data_group, const ChartAxis2D& x_axis, const ChartAxis2D& y_axis);

public:

  Chart2D();

  void setBackgroundColour(const JColour& col);
  void setMargins(int left_m, int right_m, int top_m, int bottom_m);
  void setLabelMargin(int t);
  void setNumbersMargin(int t);
  void setTickLength(int t);

  void setTitleItem(DrawItem2D* li);
  void setTitleMargin(int m);
 
  ChartAxis2D& xTopAxis() { return x_top_axis; }
  ChartAxis2D& xBottomAxis() { return x_bottom_axis; }
  ChartAxis2D& yLeftAxis() { return y_left_axis; }
  ChartAxis2D& yRightAxis() { return y_right_axis; }

  const ChartAxis2D& xTopAxis() const { return x_top_axis; }
  const ChartAxis2D& xBottomAxis() const { return x_bottom_axis; }
  const ChartAxis2D& yLeftAxis() const { return y_left_axis; }
  const ChartAxis2D& yRightAxis() const { return y_right_axis; }

  DrawGroup2D& underLinesGroup() { return under_lines_group; }
  DrawGroup2D& overLinesGroup() { return over_lines_group; }
 
  void registerData0(ChartLineData2D& cd) { registerData00(cd); }

  void registerData00(ChartLineData2D& cd) { data00_list.append(&cd); }
  void registerData01(ChartLineData2D& cd) { data01_list.append(&cd); }
  void registerData10(ChartLineData2D& cd) { data10_list.append(&cd); }
  void registerData11(ChartLineData2D& cd) { data11_list.append(&cd); }

  void removeAllData() { 
    data00_list.clear(); 
    data01_list.clear(); 
    data10_list.clear(); 
    data11_list.clear(); 
  }

  void removeData00(ChartLineData2D& cd) { data00_list.remove(&cd); }
  void removeData01(ChartLineData2D& cd) { data01_list.remove(&cd); }
  void removeData10(ChartLineData2D& cd) { data10_list.remove(&cd); }
  void removeData11(ChartLineData2D& cd) { data11_list.remove(&cd); }

  void deleteLabelItems() { label_list.clear(); }
  void addLabelItem(ChartLabel2D* item) { label_list.append(item); }

  void setKey(ChartKey2D* k) { chart_key = k; }
 
  const Vec2D plotAreaCentre() const { return plot_origin + (plot_size / 2); }
  const Vec2D plotAreaTopRight() const { return plot_origin + plot_size; }

  void arrangeDrawing(int xs, int ys);

  void drawToBuffer(JBuffer& buff) const;
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHART2D_H

