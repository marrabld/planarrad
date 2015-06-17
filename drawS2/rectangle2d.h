
// rectangle2d.h

#ifndef _JUDE_DRAWS2_RECTANGLE2D_H
#define _JUDE_DRAWS2_RECTANGLE2D_H

#include "S2/box2d.h"
#include "drawS2/drawitem2d.h"

using namespace jude::S2;

namespace jude {
namespace drawS2 {



class Rectangle2D : public DrawItem2D {

  Box2D box;
  JColour col;
 
public:

  Rectangle2D();
  Rectangle2D(const Box2D& b, const JColour& c);
  virtual ~Rectangle2D() { }
  
  void setTo(const Box2D& b);

  const JColour& colour() const { return col; }
  void setColour(const JColour& c) { col = c; }
 
  const Box2D boundingBox() const;

  void setCentrePos(const Vec2D& v);
 
  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_RECTANGLE2D_H

