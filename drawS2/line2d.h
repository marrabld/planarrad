
// line2d.h

#ifndef _JUDE_DRAWS2_LINE2D_H
#define _JUDE_DRAWS2_LINE2D_H

#include "base/jlist.h"
#include "base/jcolour.h"
#include "S2/vec2d.h"
#include "S2/box2d.h"
#include "drawS2/drawitem2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



class Line2D : public DrawItem2D {

  Vec2D p0;
  Vec2D p1;

  JColour col;
  int line_thickness;

  mutable Box2D bounding_box;

public:

  Line2D();
  Line2D(const JColour& c, int lt=1);
  Line2D(const Vec2D& p0, const Vec2D& p1, const JColour& c, int lt=1);
  virtual ~Line2D() { }

  const JColour& colour() const { return col; }
  void setColour(const JColour& c) { col = c; }

  const int thickness() const { return line_thickness; }
  void setThickness(int t) { line_thickness = t; }

  const Box2D boundingBox() const;

  void setEndPositions(const Vec2D& ip0, const Vec2D& ip1);

  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_LINE2D_H

