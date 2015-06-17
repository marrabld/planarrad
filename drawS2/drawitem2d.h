
// drawitem2d.h

#ifndef _JUDE_DRAWS2_DRAWITEM2D_H
#define _JUDE_DRAWS2_DRAWITEM2D_H

#include "S2/box2d.h"
#include "drawS2/trans2d.h"
#include "base/jbuffer.h"
#include "base/jtextstream.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



class DrawItem2D {

protected:

  bool vis;
  bool selected_flag;
 
public:

  DrawItem2D();
  virtual ~DrawItem2D();
 
  virtual void deleteAllItems();

  bool selected() const { return selected_flag; }
  void setSelected(bool f) { selected_flag = f; }

  bool visible() const { return vis; }
  void setVisible(bool f) { vis = f; }

    // return the real-valued bounding box with no pixel sized objects
  virtual const Box2D boundingBox() const = 0;
 
  virtual void setCentrePos(const Vec2D& v) { }

    // move object in real coordinate space
  //virtual void move(const Vec2D& v) = 0;

    // move object in real space but by pixel units based on the given translation
  //virtual void moveByPixel(const Trans2D& trans, int dx, int dy) = 0;
    // update the translation to incorporate pixel sized objects within the bounding box
  //virtual void expandForPixelObjects(Trans2D& trans) = 0;

  //virtual bool pixelPointOn(const Trans2D& trans, int x, int y) const = 0;
  //void selectAtPixelPoint(const Trans2D& trans, int x, int y);

  virtual void draw(const Trans2D& trans, JBuffer& buff) const = 0;
  virtual void drawEPS(const Trans2D& trans, JTextStream& str) const = 0;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_DRAWITEM2D_H


