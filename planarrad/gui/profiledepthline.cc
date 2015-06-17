
#include "profiledepthline.h"
#include "base/jlog.h"



ProfileDepthLine::ProfileDepthLine(const Chart2D& chart) :
  chart(chart),
  curr_depth(0) { }



void ProfileDepthLine::setDepth(double d) {
  curr_depth = d;
}



void ProfileDepthLine::draw(const Trans2D& trans, JBuffer& buff) const {

  //double x0 = chart.xBottomAxis().minValue();
  //double x1 = chart.xBottomAxis().maxValue();

  double x0 = chart.xBottomAxis().linePos0().x();
  double x1 = chart.xBottomAxis().linePos1().x();

  double y = chart.yLeftAxis().valueToPixelPosition(curr_depth);

  double dx = 6;
  double dy = 6;

  JColour col(180, 180, 180);

  //  jlog::ls << "here " << curr_depth << " y " << y <<  " x0 " << x0 << " x1 " << x1 << " dx " << dx << " dy " << dy << "\n";

  Line2D l1(Vec2D(x0, y), Vec2D(x1, y), col, 1); 
  Line2D l2(Vec2D(x0, y+1), Vec2D(x1, y+1), col, 1); 

  Line2D l3(Vec2D(x0, y), Vec2D(x0-dx, y-dy), col, 1); 
  Line2D l4(Vec2D(x0, y+1), Vec2D(x0-dx, y+dy+1), col, 1); 

  Line2D l5(Vec2D(x1, y), Vec2D(x1+dx, y-dy), col, 1); 
  Line2D l6(Vec2D(x1, y+1), Vec2D(x1+dx, y+dy+1), col, 1); 

  l1.draw(trans, buff);
  l2.draw(trans, buff);

  l3.draw(trans, buff);
  l4.draw(trans, buff);

  l5.draw(trans, buff);
  l6.draw(trans, buff);
}

