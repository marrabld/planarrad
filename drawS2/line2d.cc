
// line2d.cc

#include "line2d.h"
#include "trans2d.h"
#include "base/jbuffer.h"
#include <stdio.h>

using namespace jude::drawS2;



// Line2D::(constructor)

Line2D::Line2D() {
  col = JColour(0,0,0);
  line_thickness = 1;
  bounding_box.initBoundingBox();
}



// Line2D::(constructor)

Line2D::Line2D(const JColour& c, int lt) :
  col(c),
  line_thickness(lt) {

  bounding_box.initBoundingBox();
}



// Line2D::(constructor)

Line2D::Line2D(const Vec2D& p0, const Vec2D& p1, const JColour& c, int lt) :
  p0(p0),
  p1(p1),
  col(c),
  line_thickness(lt) {

  bounding_box.initBoundingBox();
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
}



// Line2D::boundingBox()

const Box2D Line2D::boundingBox() const {
  return bounding_box;
}



// Line2D::setCentrePos()

void Line2D::setEndPositions(const Vec2D& ip0, const Vec2D& ip1) {
  p0 = ip0;
  p1 = ip1;
  bounding_box.initBoundingBox();
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
}
 


// Line2D::draw()

void Line2D::draw(const Trans2D& trans, JBuffer& buff) const {

  Vec2D tv0(trans.intoTrans(p0));
  Vec2D tv1(trans.intoTrans(p1));
  
  buff.drawLine((int32) tv0.x(), (int32) tv0.y(), (int32) tv1.x(), (int32) tv1.y(), col);
}



// Line2D::draw()

void Line2D::drawEPS(const Trans2D& trans, JTextStream& str) const {

  /*
  Vec2D *pfirst = point_list.first();
  if (!pfirst) return;

  pc.stream() << ((double)line_thickness)/2 << " setlinewidth\n";
  pc.stream() << "newpath\n";
  pc.stream() << trans.realToDrawX(pfirst->x()) << " " << trans.realToDrawY(pfirst->y()) << " moveto\n";
  Vec2D *p = point_list.next();
  while (p) {
    pc.stream() << trans.realToDrawX(p->x()) << " " << trans.realToDrawY(p->y()) << " lineto\n";
    p = point_list.next();
  }
  if (closed_flag) pc.stream() << trans.realToDrawX(pfirst->x()) << " " << trans.realToDrawY(pfirst->y()) << " lineto\n";
  pc.stream() << "stroke\n";
  */
}

