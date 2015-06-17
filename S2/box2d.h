
// box2d.h

#ifndef _JUDE_S2_BOX2D_H
#define _JUDE_S2_BOX2D_H

#include "vec2d.h"

namespace jude {
namespace S2 {



class Box2D {

  Vec2D vert0;
  Vec2D vert1;

public:

  Box2D() { }

  Box2D(const Vec2D& sz) :
    vert0(0, 0),
    vert1(sz.x(), sz.y()) { }

  Box2D(const Vec2D& minp, const Vec2D& maxp) :
    vert0(minp),
    vert1(maxp) { }

  void initBoundingBox() {
    vert0 = Vec2D(DOUBLE_MAX, DOUBLE_MAX);
    vert1 = Vec2D(DOUBLE_MIN, DOUBLE_MIN);
  }
 
  void expandToInclude(const Box2D& b) {
    if (b.xMin() < vert0.x()) vert0.setX(b.xMin());
    if (b.xMax() > vert1.x()) vert1.setX(b.xMax());
    if (b.yMin() < vert0.y()) vert0.setY(b.yMin());
    if (b.yMax() > vert1.y()) vert1.setY(b.yMax());
  }

  void expandToInclude(const Vec2D& v) {
    if (v.x() < vert0.x()) vert0.setX(v.x());
    if (v.x() > vert1.x()) vert1.setX(v.x());
    if (v.y() < vert0.y()) vert0.setY(v.y());
    if (v.y() > vert1.y()) vert1.setY(v.y());
  }

  void expandToIncludeX(double x) {
    if (x < vert0.x()) vert0.setX(x);
    if (x > vert1.x()) vert1.setX(x);
  } 

  void expandToIncludeY(double y) { 
    if (y < vert0.y()) vert0.setY(y);
    if (y > vert1.y()) vert1.setY(y);
  }

  void shift(const Vec2D& s) {
    vert0 += s;
    vert1 += s;
  }

  double xMin() const { return vert0.x(); }
  double xMax() const { return vert1.x(); }
  double yMin() const { return vert0.y(); }
  double yMax() const { return vert1.y(); }

  void setXMin(double x) { vert0.setX(x); }
  void setXMax(double x) { vert1.setX(x); }
  void setYMin(double y) { vert0.setY(y); }
  void setYMax(double y) { vert1.setY(y); }

  double xSize() const { return vert1.x() -  vert0.x(); }
  double ySize() const { return vert1.y() -  vert0.y(); }
 
  double xCentre() const { return (vert0.x() + vert1.x()) / 2; }
  double yCentre() const { return (vert0.y() + vert1.y()) / 2; }
 
  const Vec2D centrePoint() const { return Vec2D(xCentre(), yCentre()); }

  const Vec2D& v0() const { return vert0; };
  const Vec2D& v1() const { return vert1; };
 
  const Vec2D& minPoint() const { return vert0; };
  const Vec2D& maxPoint() const { return vert1; };
};



} } // namespace jude::S2

#endif // _JUDE_S2_BOX2D_H

