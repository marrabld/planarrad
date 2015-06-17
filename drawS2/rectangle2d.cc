
// rectangle.cc

#include "rectangle2d.h"
#include "drawS2/trans2d.h"
#include "base/jbuffer.h"

using namespace jude::drawS2;



// Rectangle2D::(constructor)

Rectangle2D::Rectangle2D() {
  // nothing to do
}



// Rectangle2D::(constructor)

Rectangle2D::Rectangle2D(const Box2D& b, const JColour& c) {
  box = b;
  col = c;
}



// Rectangle2D::(constructor)

void Rectangle2D::setTo(const Box2D& b) {
  box = b;
}



// Rectangle2D::boundingBox()

const Box2D Rectangle2D::boundingBox() const {
  return box;
}


 
// Rectangle2D::setCentrePos()

void Rectangle2D::setCentrePos(const Vec2D& v) {
  box.shift(v - box.centrePoint());
}
 


// Rectangle2D::draw()

void Rectangle2D::draw(const Trans2D& trans, JBuffer& buff) const {

  Vec2D v0(trans.intoTrans(box.minPoint()));
  Vec2D v1(trans.intoTrans(box.maxPoint()));

	   /*
  int x0 = trans.real(box.minPoint().x());
  //int y0 = trans.drawHeight() - trans.realToDrawY(box.minPoint().y()) - 1;
  int y0 = trans.realToDrawY(box.minPoint().y());
  int x1 = trans.realToDrawX(box.maxPoint().x());
  //int y1 = trans.drawHeight() - trans.realToDrawY(box.maxPoint().y()) - 1;
  int y1 = trans.realToDrawY(box.maxPoint().y());
	   */

  int32 x0 = (int32) v0.x();
  int32 y0 = (int32) v0.y();
  int32 x1 = (int32) v1.x();
  int32 y1 = (int32) v1.y();

  buff.fillRectangle(x0, y0, x1, y1, col); 
}



// Rectangle2D::draw()

void Rectangle2D::drawEPS(const Trans2D&, JTextStream&) const {

}

