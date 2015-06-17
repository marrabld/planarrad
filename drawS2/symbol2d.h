
// symbol2d.h

#ifndef _JUDE_DRAWS2_SYMBOL2D_H
#define _JUDE_DRAWS2_SYMBOL2D_H

#include "S2/vec2d.h"
#include "drawS2/drawitem2d.h"

using namespace jude::S2;

namespace jude {
namespace drawS2 {



class Symbol2D : public DrawItem2D {

  int symbol_code;
  int pixel_size;
  int line_thickness;

  Vec2D pos;
  JColour col;
 
public:

  enum { DiagonalCross, \
	 StraightCross, \
	 Circle, \
	 FilledCircle, \
	 Triangle, \
	 FilledTriangle, \
	 Square, \
	 FilledSquare, \
	 VerticalBar, \
	 HorizontalBar };

  Symbol2D(int code, const Vec2D& p, const JColour& c, int sz=10, int t=1);
  virtual ~Symbol2D() { }

  int symbol() const { return symbol_code; }
  void setSymbol(int code) { symbol_code = code; }

  int size() const { return pixel_size; }
  void setSize(int sz) { pixel_size = sz; }

  const Vec2D& position() const { return pos; }
  void setCentrePos(const Vec2D& p) { pos = p; }

  const Box2D boundingBox() const;
 
  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_SYMBOL2D_H


