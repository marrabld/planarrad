
// lineseq2d.h

#ifndef _JUDE_DRAWS2_LINESEQ2D_H
#define _JUDE_DRAWS2_LINESEQ2D_H

#include "base/jlist.h"
#include "base/jcolour.h"
#include "S2/vec2d.h"
#include "S2/box2d.h"
#include "drawS2/drawitem2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



class LineSeq2D : public DrawItem2D {

  JList<Vec2D> point_list;
  JColour col;
  bool closed_flag;
  int line_thickness;

  mutable Box2D bounding_box;

public:

  LineSeq2D();
  LineSeq2D(const JColour& c, int lt=1);
  LineSeq2D(const Vec2D& p0, const Vec2D& p1, const JColour& c, int lt=1);
  LineSeq2D(const Vec2D& p0, const Vec2D& p1, const Vec2D& p2, const JColour& c, int lt=1);
  LineSeq2D(const Vec2D& p0, const Vec2D& p1, const Vec2D& p2, const Vec2D& p3, const JColour& c, int lt=1);
  LineSeq2D(const Vec2D& p0, const Vec2D& p1, const Vec2D& p2, const Vec2D& p3, const Vec2D& p4, const JColour& c, int lt=1);
  virtual ~LineSeq2D() { }

  const JColour& colour() const { return col; }
  void setColour(const JColour& c) { col = c; }

  const int thickness() const { return line_thickness; }
  void setThickness(int t) { line_thickness = t; }

  bool closed() const { return closed_flag; }
  void setClosed(bool f) { closed_flag = f; }

  void appendPoint(const Vec2D& p);
  void removeAllPoints();

  bool getNearestIntersection(const Vec2D& v0, const Vec2D& v1, Vec2D& vi) const;

  double realWidth() const;
  double realHeight() const;
  const Box2D boundingBox() const;
  void expandForPixelObjects(Trans2D&) { }

  bool pixelPointOn(const Trans2D& trans, int x, int y) const;
  void move(const Vec2D& v);
  void moveByPixel(const Trans2D& trans, int dx, int dy);
 
  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_LINESEQ2D_H

