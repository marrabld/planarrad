
// trans2d.h

#ifndef _JUDE_DRAWS3_TRANS2D_H
#define _JUDE_DRAWS3_TRANS2D_H

#include "S2/box2d.h"
#include "drawS2/trans2d.h"
#include "base/jbuffer.h"
#include "base/jtextstream.h"
#include <stdlib.h>

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {




// Trans2D
// translations when drawing a Drawing2D

class Trans2D {

  //Box2D bounding_box;        // defines what is to be fitted into the draw rectamgle

  // the below co-ords are used to orientate the real valued bounding box into a 2d pixel buffer
  // there x increases to the right and y increases downwards, relative to (0,0) at top left

  //int w;     // width of pixel rectangle bounding box corresponds to
  //int h;     // height of pixel rectangle bounding box corresponds to
  //int xoff;  // x-position of rectangle bounding box corresponds to, relative to top left
  //int yoff;  // y-position of rectangle bounding box corresponds to. relative to top left

  // all are inclusive
  //int x0;
  //int y0;
  //int x1;
  //int y1;

    // these are re-calculated whenever width or bounding box changes
  double x_scale_factor;
  double y_scale_factor;

  Vec2D buff_centre;

    // used when calculating how to fit in pixel objects
  //double right_excess;
  //double left_excess;
  //double top_excess;
  //double bottom_excess;

public:

  Trans2D();

  void setXOffset(int x) { buff_centre.setX(x); }
  void setYOffset(int y) { buff_centre.setY(y); }

  void shiftOffset(const Vec2D& v) { buff_centre += v; }

  double xScale() const { return x_scale_factor; }
  void setXScale(double s) { x_scale_factor = s; }

  double yScale() const { return y_scale_factor; }
  void setYScale(double s) { y_scale_factor = s; }

  void multiplyScale(double f) { x_scale_factor *= f; y_scale_factor *= f; }

  void autoSet(const Box2D& bbox, int xs, int ys);

  double intoTransY(double y) const { return (y * y_scale_factor) + buff_centre.y(); }
  double outOfTransY(double y) const { return (y -  buff_centre.y()) / y_scale_factor; }

  Vec2D intoTrans(const Vec2D& v) const { 
    return Vec2D((v.x() * x_scale_factor) + buff_centre.x(), (v.y() * y_scale_factor) + buff_centre.y()); }

  Vec2D outOfTrans(const Vec2D& v) const { 
    return Vec2D( (v.x() - buff_centre.x()) / x_scale_factor, (v.y() -  buff_centre.y()) / y_scale_factor ); }

  void display(JTextStream& str) const;
 
  //Trans2D(int x1=128, int y1=128);
  //Trans2D(int x0, int y0, int x1, int y1, bool f);
  //Trans2D(const Box2D& box, int x1, int y1);
  //Trans2D(const Box2D& box, int w, int h, int x0, int y0);
 
  //void setDrawArea(int x1, int y1);
  //void setDrawArea(int w, int h, int x0, int y0);

  //int pixelAreaX0() const { return x0; }
  //int pixelAreaX1() const { return x1; }
  //int pixelAreaY0() const { return y0; }
  //int pixelAreaY1() const { return y1; }

  //void setPixelArea(int x0, int y0, int x1, int y1);

  //Box2D& boundingBox() { return bounding_box; }
  //void setBoundingBox(const Box2D& box);

  //void resize(int nw, int nh);

  //void initPixelExcess();
  //void expandXPixelExcess(double xp, int pr, int pl);
  //void expandYPixelExcess(double yp, int pt, int pb);
  //void incorporatePixelExcess(int pixel_border);

  //void removeDistortion();

  //void updateScale();
  //void setXScale(double sx);
  //void setYScale(double sy);

  //int drawAbsWidth() const { return abs(x1 - x0) + 1; }
  //int drawAbsHeight() const { return abs(y1 - y0) + 1; }
  //int drawXOffset() const { return x0; }
  //int drawYOffset() const { return y0; }
  
  //void setWidth(int iw) { w = iw; }
  //void setHeight(int ih) { h = ih; }
  //void setXOffset(int x) { x1 += x - x0; x0 = x; }
  //void setYOffset(int y) { y1 += y - y0; y0 = y; }

  //int realToDrawX(double x) const;
  //int realToDrawY(double y) const;
  //int realToDrawY_Flip(double y) const;
  //int realToDrawDX(double dx) const;
  //int realToDrawDY(double dy) const;

  //void realToDraw(const Vec2D& v, Vec2D& tv) const;

  //double drawToRealX(int x) const;
  //double drawToRealY(int y) const;
  //double drawToRealDX(int dx) const; 
  //double drawToRealDY(int dy) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_TRANS2D_H

