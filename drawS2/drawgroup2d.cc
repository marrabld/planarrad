
// drawgroup2d.h

#include "drawgroup2d.h"
#include "S2/box2d.h"
#include <stdio.h>

using namespace jude::drawS2;



// DrawGroup2D::(constructor)

DrawGroup2D::DrawGroup2D() { 
  drawing_items.setAutoDelete(false);
}



// JDrawing2DGroup::calcBoundingBox() [protected]

//const JBox2D JDrawing2DGroup::calcCompleteBoundingBox() const {
//  JBox2D box;
//  box.initialiseBoundingBox();
//  JDrawing2DItem* item = drawing_items.first();
//  while (item) {
//    box.expandToAccomodate(item->completeBoundingBox());
//    item = drawing_items.next();
//  }
//  return box;
//}



// DrawGroup2D::addItem()

void DrawGroup2D::addItem(DrawItem2D* item) {
  drawing_items.append(item);
}
 


// DrawGroup2D::removeItem()

void DrawGroup2D::removeItem(DrawItem2D* item) {
  drawing_items.remove(item);
}



// DrawGroup2D::toTop()

void DrawGroup2D::toTop(DrawItem2D* item) {
  drawing_items.toLast(item);
}



// DrawGroup2D::toBottom()

void DrawGroup2D::toBottom(DrawItem2D* item) {
  drawing_items.toFirst(item);
}



// DrawGroup2D::removeAllItems()

void DrawGroup2D::removeAllItems() {
  drawing_items.clear();
} 



// DrawGroup2D::deleteAllItems()

void DrawGroup2D::deleteAllItems() {

  DrawItem2D* item = drawing_items.first();
  while (item) {
    item->deleteAllItems();
    drawing_items.remove(item);
    delete item;
    item = drawing_items.first();
  }
} 



// DrawGroup2D::itemCount()

int DrawGroup2D::itemCount() const {
  return drawing_items.count();
}



// JDrawing2DGroup::x()

//int JDrawing2DGroup::x() const {
//  return trans->realToDrawX(realBoundingBox().min().x());
//}



// JDrawing2DGroup::y()

//int JDrawing2DGroup::y() const {
//  return trans->realToDrawY(realBoundingBox().min().y());
//}



// JDrawing2DGroup::width()

//int JDrawing2DGroup::width() const {
//  return trans->realToDrawDX(realBoundingBox().width());
//}



// JDrawing2DGroup::height()

//int JDrawing2DGroup::height() const {
//  return trans->realToDrawDY(realBoundingBox().height());
//}
 


// DrawGroup2D::realWidth()

//double DrawGroup2D::realWidth() const {
//  return boundingBox().xSize();
//}



// DrawGroup2D::realHeight()

//double DrawGroup2D::realHeight() const {
//  return realBoundingBox().ySize();
//}
 


// DrawGroup2D::boundingBox()

const Box2D DrawGroup2D::boundingBox() const {

  Box2D box;
  box.initBoundingBox();

  DrawItem2D* item = drawing_items.first();
  while (item) {
    box.expandToInclude(item->boundingBox());
    item = drawing_items.next();
  }

  return box;
}



// DrawGroup2D::expandForPixelObjects()

//void DrawGroup2D::expandForPixelObjects(Trans2D& trans) {
//  DrawItem2D* item = drawing_items.first();
// while (item) {
//   item->expandForPixelObjects(trans);
//   item = drawing_items.next();
// }
//}



// DrawGroup2D::pointOn()

//bool DrawGroup2D::pixelPointOn(const Trans2D&, int, int) const {
// return false;
//}



// DrawGroup2D::moveByReal()

//void DrawGroup2D::move(const Vec2D& v) {
// DrawItem2D* item = drawing_items.first();
// while (item) {
//   item->move(v);
//   item = drawing_items.next();
// }
//}



// DrawGroup2D::moveByPixel()

//void DrawGroup2D::moveByPixel(const Trans2D& trans, int dx, int dy) {
// DrawItem2D* item = drawing_items.first();
// while (item) {
//   if (item->selected()) item->moveByPixel(trans,dx,dy);
//   item = drawing_items.next();
// }
//}



// DrawGroup2D::selectAtPixelPoint()

//void DrawGroup2D::selectAtPixelPoint(const Trans2D& trans, int x, int y) {
// DrawItem2D* item = drawing_items.first();
// while (item) {
//   item->selectAtPixelPoint(trans,x,y);
//   item = drawing_items.next();
// }
//}



// DrawGroup2D::draw()

void DrawGroup2D::draw(const Trans2D& trans, JBuffer& buff) const {
  DrawItem2D* item = drawing_items.first();
  while (item) {
    if (item->visible()) item->draw(trans,buff);
    item = drawing_items.next();
  }
}



// DrawGroup2D::draw()

void DrawGroup2D::drawEPS(const Trans2D& trans, JTextStream& str) const {
  DrawItem2D* item = drawing_items.first();
  while (item) {
    if (item->visible()) item->drawEPS(trans, str);
    item = drawing_items.next();
  }
}

