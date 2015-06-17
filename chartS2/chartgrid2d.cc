
// chartgrid2d.h

#include "chartS2/chartgrid2d.h"

using namespace jude::chartS2;



ChartGrid2D::ChartGrid2D() :
  grid_col(200,200,200) {

} 



ChartGrid2D::~ChartGrid2D() {
  // nothing to do
}



void ChartGrid2D::setAxes(const ChartAxis2D& x_axis, const ChartAxis2D& y_axis) {

  top_draw_group.deleteAllItems();

  double y0 = y_axis.valueToPixelPosition(y_axis.minValue());
  double y1 = y_axis.valueToPixelPosition(y_axis.maxValue());
   
  for (int i=0; i<x_axis.tickLocationCount(); i++) {
    double x = x_axis.valueToPixelPosition(x_axis.tickLocation(i));
    top_draw_group.addItem(new Line2D(Vec2D(x,y0), Vec2D(x,y1), grid_col));
  }
  
  double x0 = x_axis.valueToPixelPosition(x_axis.minValue());
  double x1 = x_axis.valueToPixelPosition(x_axis.maxValue());
  
  for (int i=0; i<y_axis.tickLocationCount(); i++) {
    double y = y_axis.valueToPixelPosition(y_axis.tickLocation(i));
    top_draw_group.addItem(new Line2D(Vec2D(x0,y), Vec2D(x1,y), grid_col));
  }
}

