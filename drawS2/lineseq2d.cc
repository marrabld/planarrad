
// lineseq2d.cc

#include "lineseq2d.h"
#include "trans2d.h"
#include "base/jbuffer.h"
#include <stdio.h>

using namespace jude::drawS2;



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D() : 
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  col = JColour(0,0,0);
  closed_flag = false;
  line_thickness = 1;
}



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D(const JColour& c, int lt) :
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  col = c;
  closed_flag = false;
  line_thickness = lt;
}



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D(const Vec2D& p0, const Vec2D& p1, const JColour& c, int lt) :
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  point_list.append(new Vec2D(p0));
  point_list.append(new Vec2D(p1));
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
  col = c;
  closed_flag = false;
  line_thickness = lt;
}



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D(const Vec2D& p0, const Vec2D& p1,  const Vec2D& p2, const JColour& c, int lt) :
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  point_list.append(new Vec2D(p0));
  point_list.append(new Vec2D(p1));
  point_list.append(new Vec2D(p2));
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
  bounding_box.expandToInclude(p2);
  col = c;
  closed_flag = false;
  line_thickness = lt;
}



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D(const Vec2D& p0, const Vec2D& p1,  const Vec2D& p2, const Vec2D& p3, const JColour& c, int lt) :
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  point_list.append(new Vec2D(p0));
  point_list.append(new Vec2D(p1));
  point_list.append(new Vec2D(p2));
  point_list.append(new Vec2D(p3));
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
  bounding_box.expandToInclude(p2);
  bounding_box.expandToInclude(p3);
  col = c;
  closed_flag = false;
  line_thickness = lt;
}



// LineSeq2D::(constructor)

LineSeq2D::LineSeq2D(const Vec2D& p0, const Vec2D& p1,  const Vec2D& p2, const Vec2D& p3, const Vec2D& p4, const JColour& c, int lt) :
  bounding_box(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10)) {

  point_list.setAutoDelete(true);
  point_list.append(new Vec2D(p0));
  point_list.append(new Vec2D(p1));
  point_list.append(new Vec2D(p2));
  point_list.append(new Vec2D(p3));
  point_list.append(new Vec2D(p4));
  bounding_box.expandToInclude(p0);
  bounding_box.expandToInclude(p1);
  bounding_box.expandToInclude(p2);
  bounding_box.expandToInclude(p3);
  bounding_box.expandToInclude(p4);
  col = c;
  closed_flag = false;
  line_thickness = lt;
}



// LineSeq2D::appendPoint()

void LineSeq2D::appendPoint(const Vec2D& p) {
  point_list.append(new Vec2D(p));
  bounding_box.expandToInclude(p);
}



// LineSeq2D::removeAllPoints()

void LineSeq2D::removeAllPoints() {
  if (point_list.count()>0) point_list.clear();
  bounding_box = Box2D(Vec2D(1E10,1E10),Vec2D(-1E10,-1E10));
  closed_flag = false;
}



// JLineSequence2D::x()

//int JLineSequence2D::x() const {
//  return trans->realToDrawX(bounding_box.min().x());
//}



// JLineSequence2D::y()

//int JLineSequence2D::y() const {
//  return trans->realToDrawY(bounding_box.min().y());
//}



// JLineSequence2D::width()

//int JLineSequence2D::width() const {
//  return trans->realToDrawDX(bounding_box.width());
//}



// JLineSequence2D::height()

//int JLineSequence2D::height() const {
//  return trans->realToDrawDY(bounding_box.height());
//}



//double LineSeq2D::realWidth() const {
//  return realBoundingBox().width();
//}


//double LineSeq2D::realHeight() const {
//  return realBoundingBox().height();
//} 



// LineSeq2D::pointOn()

bool LineSeq2D::pixelPointOn(const Trans2D&, int, int) const {
  return false;
}



// LineSeq2D::moveByPixel()

//void LineSeq2D::moveByPixel(const Trans2D& trans, int dx, int dy) {
//  Vec2D *p = point_list.first();
// while (p) {
//  (*p) += Vec2D(trans.drawToRealDX(dx), trans.drawToRealDY(dy));
//   p = point_list.next();
// }
//}
 


// LineSeq2D::moveByReal()

//void LineSeq2D::move(const Vec2D& v) {
// Vec2D *p = point_list.first();
// while (p) {
//   (*p) += Vec2D(v.x(),v.y());
//   p = point_list.next();
// }
//}
 


// LineSeq2D::boundingBox()

const Box2D LineSeq2D::boundingBox() const {
  return bounding_box;
}



// JLabel2D::completeBoundingBox()

//const JBox2D JLineSequence2D::completeBoundingBox() const {
//  return bounding_box;
//}




// LineSeq2D::getNearestIntersection()
/*
bool LineSeq2D::getNearestIntersection(const Vec2D& v0, const Vec2D& v1, Vec2D& vi) const {

  vi = v1;
  double dmin = v0.distanceSQ(v1);
  bool flag = false;
  Vec2D v; 

  Vec2D *p0 = point_list.first();
  Vec2D *p1 = point_list.next();

  while (p1) {
    //if (Vec2D::intersect(*p0,*p1,v0,v1,v)) {
    //  double d = v0.squaredDistance(v);
    //  if (d<dmin) { dmin=d; vi=v; flag=true; }
    // }
    p0 = p1;
    p1 = point_list.next();
  }

  return flag;
}
*/


// LineSeq2D::draw()

void LineSeq2D::draw(const Trans2D& trans, JBuffer& buff) const {

  Vec2D *p0 = point_list.first();
  if (!p0) return;

  Vec2D *p1 = point_list.next();

  Vec2D first(trans.intoTrans(*p0));

  Vec2D tv0(first);
  Vec2D tv1;

  while (p1) { 
    tv1 = trans.intoTrans(*p1);
    buff.drawLine((int32) tv0.x(), (int32) tv0.y(), (int32) tv1.x(), (int32) tv1.y(), col);
    p0 = p1;
    tv0 = tv1;
    p1 = point_list.next();
  }

  if (closed_flag) buff.drawLine((int32) tv0.x(), (int32) tv0.y(), (int32) first.x(), (int32) first.y(), col);
}



// LineSeq2D::draw()

void LineSeq2D::drawEPS(const Trans2D& trans, JTextStream& str) const {

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

