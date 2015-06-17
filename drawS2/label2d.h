
// label2d.h

#ifndef _JUDE_DRAWS2_LABEL2D_H
#define _JUDE_DRAWS2_LABEL2D_H

#include "base/jcolour.h"
#include "S2/vec2d.h"
#include "drawS2/drawitem2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



class Label2D : public DrawItem2D {

  Vec2D pos;
  //JFont f;
  JString t;
  JColour col;
  JColour back_col;
  int rotate;
  int point_location; 
  int edge_border;
  Vec2D edge_vec; 

  int x;
  int y;
  int dx;
  int dy;

public:

  static const int Left = 1;
  static const int Right = 2;
  static const int HCentre = 4;
  static const int Top = 8;
  static const int Bottom = 16;
  static const int VCentre = 32;
  static const int Edge = 64;
 
  Label2D();
  Label2D(const JString& s, const JColour& c);
  Label2D(const JString& s, const Vec2D& p, const JColour& c);
  //Label2D(const JString& s, const Vec2D& p, const JColour& c, const JFont& jf);
  virtual ~Label2D() { }

  //JFont& font() { return f; }

  void setAs(const JString& s, const JString& family, int size=12, bool bold=false, bool ital=false);
  void setPointLocation(int c, const Vec2D& vec=Vec2D(0,0), int border=0);

  const JString& text() const { return t; }
  void setText(const JString& s) { t = s; }

  const JColour& colour() const { return col; }
  void setColour(const JColour& c) { col = c; }

  const JColour& backgroundColour() const { return back_col; }
  void setBackgroundColour(const JColour& c) { back_col = c; }
 
  const Vec2D& position() const { return pos; }
  void setPosition(const Vec2D& p) { pos = p; }

  int rotation() const { return rotate; }
  void setRotation(int r) { rotate = r; }

  //void setFont(const JFont& font) { f = font; }
  void setFontSize(int s);
  void setFontType(const JString& type);

  int drawWidth() const;
  int drawHeight() const;

  //double realWidth() const;
  //double realHeight() const;

  const Box2D boundingBox() const;
  void expandForPixelObjects(Trans2D& trans);

  bool pixelPointOn(const Trans2D& trans, int x, int y) const;
  void move(const Vec2D& v);
  void moveByPixel(const Trans2D& trans, int dx, int dy);
 
  void draw(const Trans2D& trans, JBuffer& buff) const;
  void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::S2

#endif // _JUDE_DRAWS2_LABEL2D_H


