
// jbuffer.cc

#include <stdio.h>
#include "jbuffer.h"

using namespace jude::base;



// JBuffer::(constructor)

JBuffer::JBuffer() : 
  w(0), 
  h(0),
  data_ptr(0),
  has_alpha_buffer(false) { }
  


// JBuffer::(constructor)

JBuffer::JBuffer(int w, int h) : 
  w(w), 
  h(h),
  data_ptr(new uns32[w*h]),
  has_alpha_buffer(false) { }
  


// JBuffer::(constructor)
/*
JBuffer::JBuffer(int w, int h, uns32 *buff) : 
  w(w), 
  h(h),
  buffer(buff),
  has_alpha_buffer(false),
  created_space(false) { }
*/


// JBuffer::(destructor)

JBuffer::~JBuffer() {
  delete[] data_ptr;
}



// JBuffer::setAlphaBuffer()

void JBuffer::setAlphaBuffer(bool f) {
  has_alpha_buffer = f;
}



/*! \brief Attempts to resize the buffer to the requested width and height, \a nw, \a nh. Returns true if successful.

    The base class JBuffer will always resize and re-allocate memory accordingly. However derived classes may refuse 
    to resize because it may only be able to work at a fixed size. Therefore any operations exploting polymorphism with JBuffer 
    as a base class must always use width() and height() to get the actual size of a buffer and be able to work regardless of the size.

    The contents of the buffer are undefined after this operation.
*/

bool JBuffer::requestResize(int nw, int nh) {
    // if already this size then quit
  if (nw==w && nh==h) return true;
    // make sure inputs aren't negative
  if (nw < 0) nw = 0; 
  if (nh < 0) nh = 0;    
    // save the new width and height
  w = nw;
  h = nh;
    // reallocate
  delete[] data_ptr;
  data_ptr = new uns32[w*h];

  return true;
}



// JBuffer::isNull()
/**
Returns true if either the width or the height of the buffer is zero.
*/

bool JBuffer::isNull() const {
  return (w==0 || h==0);
}



// JBuffer::fill()

void JBuffer::fill(uns32 v) {
  uns32 *p = data_ptr;
  for (int len=w*h;len>0;len--) *(p++) = v;
}



// JBuffer::shiftBlock()

void JBuffer::shiftBlock(int xs, int ys, int xe, int ye, int dx, int dy) {

  if (!data_ptr) return;

  if (xs < 0) xs = 0; 
  if (ys < 0) ys = 0;

  if (xe > w) xe = w; 
  if (ye > h) ye = h;

    // dx < 0, dy < 0
  if (xs + dx < 0) xs = -dx;
  if (ys + dy < 0) ys = -dy;

    // dx > 0. dy > 0
  if (xe + dx > w) xe = w - dx; 
  if (ye + dy > h) ye = h - dy; 

  if (xs >= xe || ys >= ye) return;

  int px, py;
  uns32* from_ptr;
  uns32* to_ptr;

  if (dx <= 0) {

    if (dy <= 0) {
      px = 1;
      py = w - (xe - xs);
      from_ptr = data_ptr + ys * w + xs;
      to_ptr = data_ptr + (ys + dy) * w + (xs + dx);
    } else {
      px = 1;
      py = - w - (xe - xs);
      from_ptr = data_ptr + (ye - 1) * w + xs;
      to_ptr = data_ptr + (ye - 1 + dy) * w + (xs + dx);
    }

  } else {

    if (dy <= 0) {
      px = -1;
      py = w + (xe - xs);
      from_ptr = data_ptr + ys * w + xe - 1;
      to_ptr = data_ptr + (ys + dy) * w + (xe - 1 + dx);
    } else {
      px = -1;
      py = (xe - xs) - w;
      from_ptr = data_ptr + (ye - 1) * w + xe - 1;
      to_ptr = data_ptr + (ye - 1 + dy) * w + (xe - 1 + dx);
    }
  }

  //printf("xs %d ys %d xe %d ye %d dx %d dy %d w %d px %d py %d\n",xs,ys,xe,ye,dx,dy,w,px,py);

  for (int y=ys; y<ye; y++, to_ptr+=py, from_ptr+=py) {
    for (int x=xs; x<xe; x++, to_ptr+=px, from_ptr+=px) {
      *to_ptr = *from_ptr;
    }
  }

}



// Buffer2D::fillRectangle()

void JBuffer::fillRectangle(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col) {

    // sort to upper left corner and lower right corner
  int32 t;
  if (x0>x1) { t=x0; x0=x1; x1=t; } 
  if (y0>y1) { t=y0; y0=y1; y1=t; } 
  
    // clip to buffer
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x0 > width()) x0 = width();
  if (y0 > height()) y0 = height();

  //printf("fill rect %d %d %d %d\n",x0,y0,x1,y1);

  // get width and height
  int32 w = x1 - x0;
  int32 h = y1 - y0;

    // quit if nothing to draw
  if (w<=0 || h<=0) return;

    // colour as a 32 bit value
  uns32 cv = col.rgb();

    // place to start in buffer
  uns32* base = data() + y0 * width() + x0;

  for (;h>0;h--) {
    for (int i=w;i>0;i--) *(base++) = cv;
    base += width() - w;
  }
}



void JBuffer::fillEllipse(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col) {

    // sort to upper left corner and lower right corner
  int32 t;
  if (x0>x1) { t=x0; x0=x1; x1=t; } 
  if (y0>y1) { t=y0; y0=y1; y1=t; } 

  double orig_half_w = (x1 - x0) / 2.0;
  double orig_half_h = (y1 - y0) / 2.0;

  double x_centre((x1+x0) / 2.0);
  double y_centre((y1+y0) / 2.0);

    // clip to buffer
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > width()) x1 = width();
  if (y1 > height()) y1 = height();

  //printf("fill rect %d %d %d %d\n",x0,y0,x1,y1);

  // get width and height
  int32 w = x1 - x0;
  int32 h = y1 - y0;

    // quit if nothing to draw
  if (w<=0 || h<=0) return;

    // colour as a 32 bit value
  uns32 cv = col.rgb();

    // place to start in buffer
  uns32* base = data() + y0 * width() + x0;

  for (int j=h;j>0;j--) {
    for (int i=w;i>0;i--) {
      double dx = (x0 + i - x_centre) / orig_half_w;
      double dy = (y0 + j - y_centre) / orig_half_h;
      if (dx*dx + dy*dy < 1) *(base) = cv;
      base++;
    } 
    base += width() - w;
  }
}



// Buffer2D::drawLine()

void JBuffer::drawLine(int32 x0, int32 y0, int32 x1, int32 y1, const JColour& col) {

  const int BP = 15;

  x0 <<= BP;
  y0 <<= BP;
  x1 <<= BP;
  y1 <<= BP;

    // check if completely offscreen  
  int h,p,pe,len,n;
  int x_fac;
  
   // colour as a 32 bit value
  uns32 cv = col.rgb();

  int t;
    // sort vertices into top down order
  if (y0>y1) { t=y1; y1=y0; y0=t; t=x1; x1=x0; x0=t; }  

  //printf("line %d %d %d %d\n",x0>>IVec2D::BP,y0>>IVec2D::BP,x1>>IVec2D::BP,y1>>IVec2D::BP);

    // height
  y0 >>= BP;
  h = (y1 >> BP) - y0;
  uns32* base = data() + y0 * width();

    // if a flat line
  if (h==0) {

    if (y0<0 || y0>=height()) return;
	
    p = x0 >> BP;
    pe = x1 >> BP;
    len = pe-p;
    if (len==0) return;
    if (len>0) {
      for (; p<pe; p++) if (p>=0 && p<width()) base[p] = cv;
    } else {
      for (; p>pe; p--) if (p>=0 && p<width()) base[p] = cv;
    }
    return;
  } 

  x_fac = (x1 - x0) / (h+1);
 
    // if just a stack of single points
  if (x_fac <= (1<<BP) && x_fac >= -(1<<BP)) {

    for (n=0;n<=h;n++,y0++) {
      if (y0>=0 && y0<height()) {
	p = x0 >> BP;
	if (p>=0 && p<width()) base[p] = cv;
      }
      x0 += x_fac;
      base += width();
    }

    // otherwise some horizontal lines to draw
  } else {

        // going to the right
    if (x_fac>0) {

      for (n=0;n<=h;n++,y0++) {
	p = x0 >> BP;
        x0 += x_fac;
        pe = x0 >> BP;
	if (y0>=0 && y0<height()) {
	  len = pe-p;
	  for (; p!=pe; p++) if (p>=0 && p<width()) base[p] = cv;
	}
	base += width();
      }

        // going to the left
    } else {

      for (n=0;n<=h;n++,y0++) {	
	p = x0 >> BP;
	x0 += x_fac;
	pe = x0 >> BP;
	if (y0>=0 && y0<height()) {
	  len = p-pe;
	  for (; p!=pe; p--) if (p>=0 && p<width()) base[p] = cv;
	}
	base += width();
      }
    }
  }
}





void JBuffer::drawData_ARGB32(uns32* data_ptr, int x_size, int y_size, int sx, int sy, int dx, int dy, int aw, int ah) {

  if (dx < 0) { sx-=dx; aw+=dx; dx=0; }
  if (dy < 0) { sy-=dy; ah+=dy; dy=0; }
  if (dx+aw > width()) { aw = width()-dx; }
  if (dy+ah > height()) { ah = height()-dy; }

    // trim to source buffer
  if (aw > x_size-sx) aw = x_size-sx;
  if (ah > y_size-sy) ah = y_size-sy;
    // trim to destination buffer
  if (aw > width()-dx) aw = width()-dx;
  if (ah > height()-dy) ah = height()-dy;

  uns32* dest_base = data() + dy * width() + dx;
  uns32* src_base = data_ptr + sy * x_size + sx;

  for (int i=0;i<ah;i++) {
    uns32* dest = dest_base;
    uns32* src = src_base;
    for (int j=0;j<aw;j++,dest++,src++) if ((*src) & 0xff000000) *dest = *src;
    dest_base += width();
    src_base += x_size;
  }
}

