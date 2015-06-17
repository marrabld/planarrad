
// trans2d.cc

#include <stdio.h>
#include "trans2d.h"

using namespace jude::drawS2;

Trans2D::Trans2D() :
  x_scale_factor(1),
  y_scale_factor(1),
  buff_centre(0,0) { }



void Trans2D::autoSet(const Box2D& bbox, int xs, int ys) {

  x_scale_factor = (xs * 0.9) / bbox.xSize();
  y_scale_factor = (ys * 0.9) / bbox.ySize();

  buff_centre.setX((xs / 2) - bbox.xCentre() * x_scale_factor);
  buff_centre.setY((ys / 2) - bbox.yCentre() * y_scale_factor);
}



// Trans2D::(constructor)
/*
Trans2D::Trans2D(int x0, int y0, int x1, int y1, bool) : 
  bounding_box(Vec2D(0,0),Vec2D(1,1)), 
  x0(x0), 
  y0(y0),
  x1(x1), 
  y1(y1) {

  updateScale();
}

 

// Trans2D::resize()
// todo - depreceated

//void Trans2D::resize(int nw, int nh) {
//  x1 = x0 + nw;
//  y1 = y0 + nh;
//}



// Trans2D::setDrawArea()


//void Trans2D::setPixelArea(int ix0, int iy0, int ix1, int iy1) {
//  x0 = ix0;
//  y0 = iy0;
//  x1 = ix1;
//  y1 = iy1; 
//  updateScale();
//}



// Trans2D::setBoundingBox()

//void Trans2D::setBoundingBox(const Box2D& box) { 
//  bounding_box = box; 
// updateScale(); 
//}

			  


void Trans2D::setDrawArea(int w, int h, int ix, int iy) {
  x0 = ix;
  y0 = iy;
  x1 = x0 + w;
  y1 = y0 + h;
}



// Trans2D::initPixelExcess()

void Trans2D::initPixelExcess() {
  right_excess = 0;
  left_excess = 0;
  top_excess = 0;
  bottom_excess = 0;
}



// Trans2D::expandXPixelExcess()

void Trans2D::expandXPixelExcess(double xp, int pr, int pl) {
  double xr = xp + drawToRealDX(pr) - boundingBox().xMax();
  double xl = drawToRealDX(pl) + boundingBox().xMin() - xp;
  if (xr>right_excess) right_excess = xr;
  if (xl>left_excess) left_excess = xl;
}



// Trans2D::expandYPixelExcess()

void Trans2D::expandYPixelExcess(double yp, int pt, int pb) {
  double yt = yp + drawToRealDY(pt) - boundingBox().yMax();
  double yb = drawToRealDY(pb) + boundingBox().yMin() - yp;
  if (yt>top_excess) top_excess = yt;
  if (yb>bottom_excess) bottom_excess = yb;
}



// Trans2D::incorporatePixelExcess()

void Trans2D::incorporatePixelExcess(int pixel_border) {
  right_excess += pixel_border;
  left_excess += pixel_border;
  top_excess += pixel_border;
  bottom_excess += pixel_border;
  double w = bounding_box.xSize();
  double re = (w*right_excess) / (w-right_excess-left_excess);
  double le = (w*left_excess) / (w-right_excess-left_excess);
  boundingBox().expandToIncludeX(boundingBox().xMin() - le);
  boundingBox().expandToIncludeX(boundingBox().xMax() + re);
  double h = bounding_box.ySize();
  double te = (h*top_excess) / (h-top_excess-bottom_excess);
  double be = (h*bottom_excess) / (h-top_excess-bottom_excess);
  boundingBox().expandToIncludeY(boundingBox().yMin() - be);
  boundingBox().expandToIncludeY(boundingBox().yMax() + te);
}
*/


// Trans2D::removeDistortion()
/**
Forces the x-scaling and y-scaling to be the same to remove any vertical or horizantal stretching.
Both x and y scaling factors are set to the smaller of the current x-scaling and y-scaling, and the
image bounding box is shifted to centre the new smaller rendering in the view.
*/
/*
void Trans2D::removeDistortion() {  
  if (fabs(scalex) > fabs(scaley)) {
    scalex = scaley;
    double old_width = bounding_box.xSize();
    double new_width = (x1 - x0 + 1) / scalex; 
    double dx = (new_width - old_width) / 2;
    bounding_box.setXMin(bounding_box.xMin() - dx);
    bounding_box.setXMax(bounding_box.xMax() + dx);
  } else {
    scaley = scalex;
    double old_height = bounding_box.ySize();
    double new_height = (y1 - y0 + 1) / scaley; 
    double dy = (new_height - old_height) / 2;
    bounding_box.setYMin(bounding_box.yMin() - dy);
    bounding_box.setYMax(bounding_box.yMax() + dy);
  }
}
*/


// Trans2D::updateScale()
/*
void Trans2D::updateScale() {
  scalex = (x1 - x0 + 1) / bounding_box.xSize();
  scaley = (y1 - y0 + 1) / bounding_box.ySize();
}
*/


// Trans2D::setXScale()
/**
Set the x-scaling factor.
*/

//void Trans2D::setXScale(double sx) {
//  scalex = sx;
//}



// Trans2D::setYScale()
/**
Set the y-scaling factor.
*/

//void Trans2D::setYScale(double sy) {
//  scaley = sy;
//}



// Trans2D::realToDrawX()
/**
Returns the x-position in the draw co-ordinate system (integer) corresponding to the real co-ordinate x-position <i>x</i>. 
*/

//int Trans2D::realToDrawX(double x) const { 
// return x0 + (int)((x - bounding_box.xMin()) * scalex + 0.5); 
//}
  


// Trans2D::realToDrawY()
/**
Returns the y-position in the draw co-ordinate system (integer) corresponding to the real co-ordinate y-position <i>y</i>. 
*/

//int Trans2D::realToDrawY(double y) const { 
//  return y0 + (int)((y - bounding_box.yMin()) * scaley + 0.5); 
//}


/*
int Trans2D::realToDrawY_Flip(double y) const { 
  return (y1 - y0) - (y0 + (int)((y - bounding_box.minPoint().y()) * scaley + 0.5)); 
}
*/


// Trans2D::realToDraw()
/*
void Trans2D::realToDraw(const Vec2D& v, Vec2D& tv) const {
  //tv.setX((int32) ((x0 + ((v.x() - bounding_box.minPoint().x()) * scalex + 0.5))*(1<<IVec2D::BP))); 
  //tv.setY((int32) ((y0 + ((v.y() - bounding_box.minPoint().y()) * scaley + 0.5))*(1<<IVec2D::BP))); 
  tv.setX(((x0 + ((v.x() - bounding_box.xMin()) * scalex + 0.5)))); 
  tv.setY(((y0 + ((v.y() - bounding_box.yMin()) * scaley + 0.5)))); 
}
*/


// Trans2D::realToDrawDX()
/**
Returns the x-wise distance in the draw co-ordinate system (integer) corresponding to the real co-ordinate system x-wise distance <i>dx</i>. 
*/

//int Trans2D::realToDrawDX(double dx) const { 
//  return (int)((dx * scalex) + 0.5); 
//}



// Trans2D::realToDrawDY()
/**
Returns the y-wise distance in the draw co-ordinate system (integer) corresponding to the real co-ordinate system y-wise distance <i>dy</i>. 
*/

//int Trans2D::realToDrawDY(double dy) const { 
// return (int)((dy * scaley) + 0.5); 
//}



// Trans2D::drawToRealX()
/**
Returns the x-position in the real co-ordinate system (double) corresponding to the draw co-ordinate x-position <i>x</i>. 
*/

//double Trans2D::drawToRealX(int x) const { 
//  return ((x - x0) / scalex) + bounding_box.xMin(); 
//}
  


// Trans2D::drawToRealY()
/**
Returns the y-position in the real co-ordinate system (double) corresponding to the draw co-ordinate y-position <i>y</i>. 
*/

//double Trans2D::drawToRealY(int y) const { 
//  return ((y - y0) / scaley) + bounding_box.yMin(); 
//}



// Trans2D::drawToRealDX()
/**
Returns the x-wise distance in the real co-ordinate system (integer) corresponding to the draw co-ordinate system x-wise distance <i>dx</i>. 
*/

//double Trans2D::drawToRealDX(int dx) const { 
//  return dx / scalex; 
//}



// Trans2D::drawToRealDY()
/**
Returns the y-wise distance in the real co-ordinate system (integer) corresponding to the draw co-ordinate system y-wise distance <i>dy</i>. 
*/

//double Trans2D::drawToRealDY(int dy) const { 
// return dy / scaley; 
//}



// Trans2D::display()

void Trans2D::display(JTextStream& str) const {
  //printf("JTrans2D\n");
  //printf("real bbox: ");
  //bounding_box.display();
  //printf("draw x0 %d y0 %d x1 %d y1 %d\n",x0,y0,x1,y1);
  //printf("scalex %f scaley %f\n",scalex,scaley);
}

