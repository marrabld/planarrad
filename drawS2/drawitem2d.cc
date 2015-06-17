
// drawitem2d.cc

#include "drawitem2d.h"

using namespace jude::drawS2;



// DrawItem2D::(constructor)

DrawItem2D::DrawItem2D() :
  vis(true),
  selected_flag(false) { }



// DrawItem2D::(destructor)

DrawItem2D::~DrawItem2D() {
  // nothing to do
} 



// DrawItem2D::deleteAllItems()

void DrawItem2D::deleteAllItems() {
  // nothing to do
}


// DrawItem2D::selectAtRealPoint()

//void DrawItem2D::selectAtPixelPoint(const Trans2D& trans, int x, int y) {
//  if (pixelPointOn(trans,x,y)) selected_flag = !selected_flag;
//}

