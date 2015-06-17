
// label2d.cc

#include "label2d.h"
#include "trans2d.h"
#include "base/jbuffer.h"
//#include "jpainter.h"
//#include "jpixmap.h"
//#include "jimage.h"
//#include "buffer2d.h"
#include <stdio.h>
#include <math.h>

using namespace jude::drawS2;



// Label2D::(constructor)

//Label2D::Label2D() : f("Helvetiva",10) {

Label2D::Label2D() {
  t = "";
  col = JColour(0,0,0);
  back_col = JColour(255,255,255);
  //point_location = VCentre | HCentre;
  rotate = 0;
  point_location = Left | Bottom;
}



// Label2D::(constructor)

//Label2D::Label2D(const JString& s, const JColour& c) : f("Helvetiva",10) {

Label2D::Label2D(const JString& s, const JColour& c) {
  t = s;
  col = c;
  back_col = JColour(255,255,255);
  rotate = 0;
  //point_location = VCentre | HCentre;
  point_location = Left | Bottom;
}



// Label2D::(constructor)

//Label2D::Label2D(const JString& s, const Vec2D& p, const JColour& c) : f("Helvetica",10) {

Label2D::Label2D(const JString& s, const Vec2D& p, const JColour& c) {
  t = s;
  pos = p;
  col = c;
  back_col = JColour(255,255,255);
  rotate = 0;
  //point_location = VCentre | HCentre;
  point_location = Left | Bottom;
}



// Label2D::(constructor)
/*
Label2D::Label2D(const JString& s, const Vec2D& p, const JColour& c, const JFont& jf) : f(jf) {
  t = s;
  pos = p;
  col = c;
  back_col = JColour(255,255,255);
  rotate = 0;
  //point_location = VCentre | HCentre;
  point_location = Left | Bottom;
}
*/



// Label2D::setAs()

void Label2D::setAs(const JString& s, const JString& family, int size, bool bold, bool ital) {
  t = s;
  //f.setFamily(family);
  //f.setPointSize(size);
  //f.setWeight(bold ? JFont::Bold : JFont::Normal);
  //f.setItalic(ital);
}



// Label2D::setPointLocation()

void Label2D::setPointLocation(int c, const Vec2D& vec, int border) {
  point_location = c;
  edge_vec = vec;
  edge_border = border;
}



// Label2D::width()

int Label2D::drawWidth() const {
  //return (rotate==270) ?  f.height() : f.width(t);
  return 10;
}



// Label2D::height()

int Label2D::drawHeight() const {
  //return (rotate==270) ? f.width(t) : f.height();
  return 10;
}



// Label2D::realWidth()

//double Label2D::realWidth() const {
//  return 0;
//}



// Label2D::realHeight()

//double Label2D::realHeight() const {
//  return 0;
//}

 

// Label2D::realBoundingBox()

const Box2D Label2D::boundingBox() const {
  return Box2D(pos, pos);
}



// Label2D::expandForPixelObjects()

void Label2D::expandForPixelObjects(Trans2D& trans) {

  /*
  int xr_pixel, xl_pixel, yt_pixel, yb_pixel;
  int w = drawWidth();
  int h = drawHeight();

  double x = pos.x();
  double y = pos.y();

  if (point_location & Edge) {
    double vd = trans.drawToRealDY((h>>1) + edge_border);
    double hd =  trans.drawToRealDX((w>>1) + edge_border);
    double fdx = fabs(edge_vec.x());
    double fdy = fabs(edge_vec.y());
      // top or bottom intersect
    if (fdy*hd > vd*fdx) {
      x += ((edge_vec.x()*vd)/fdy);
      y += (edge_vec.y()<0) ? -vd : vd;
      // left or right intersect
    } else {
      y += ((edge_vec.y()*hd)/fdx);
      x += (edge_vec.x()<0) ? -hd : hd;
    }
  }

  if (point_location & Left) { xr_pixel=w; xl_pixel=0; }
  else if (point_location & (HCentre | Edge)) { xr_pixel=xl_pixel=w>>1; }
  else { xr_pixel=0; xl_pixel=w; }
  trans.expandXPixelExcess(x, xr_pixel, xl_pixel);
  if (point_location & Top) { yb_pixel=h; yt_pixel=0; }
  else if (point_location & (VCentre | Edge)) { yb_pixel=yt_pixel=h>>1; }
  else { yb_pixel=0; yt_pixel=h; }
  trans.expandYPixelExcess(y, yt_pixel, yb_pixel);

  dx = 0;
  dy = 0;

  if (point_location & Edge) {
    int vd = (drawHeight()>>1) + edge_border;
    int hd = (drawWidth()>>1) + edge_border;
    double fdx = fabs(edge_vec.x());
    double fdy = fabs(edge_vec.y());
    dx = - (drawWidth()>>1);
    dy = drawHeight()>>1;
         // top or bottom intersect
    if (fdy*trans.drawToRealDX(hd) > trans.drawToRealDY(vd)*fdx) {
      dx += trans.realToDrawDX(((edge_vec.x()*trans.drawToRealDY(vd))/fdy));
      dy += (edge_vec.y()<0) ? vd : -vd;
	// left or right intersect
    } else {
      dy -= trans.realToDrawDY(((edge_vec.y()*trans.drawToRealDX(hd))/fdx));
      dx += (edge_vec.x()<0) ? -hd : hd;
    }

  } else {
    if (point_location & HCentre) dx = - (drawWidth()>>1);
    else if (point_location & Right) dx = -drawWidth();
    if (point_location & VCentre) dy = drawHeight()>>1;
    else if (point_location & Top) dy = drawHeight();
  }

  */
}



// Label2D::moveByReal()

void Label2D::move(const Vec2D& v) {
  pos += Vec2D(v.x(), v.y());
}



// Label2D::moveBy()

//void Label2D::moveByPixel(const Trans2D& trans, int dx, int dy) {
//  pos += Vec2D(trans.drawToRealDX(dx), trans.drawToRealDY(dy));
//}



// Label2D::draw()

void Label2D::draw(const Trans2D& trans, JBuffer& buff) const {

  /*
  x = trans.realToDrawX(pos.x());
  //y = trans.drawHeight() - trans.realToDrawY(pos.y()) - drawHeight();
  y = trans.realToDrawY(pos.y());

  //printf("label %s at %f %f > %d %d\n",t.latin1(),pos.x(),pos.y(),x,y);
  //trans.display();
  //return;

  JPixmap pm(drawWidth(), drawHeight());
  pm.fill(back_col);
  JPainter p(&pm);

  p.setPen(col);
  p.setFont(f);
  if (rotate==270) {
    p.rotate(270);
    p.drawText(1-drawHeight(), drawWidth()-f.descent()-1, t);
  } else {
    //p.drawText(0, ,f.descent()-1, t);
    p.drawText(0, drawHeight()-f.descent()-1, t);
  }

  JImage pi(pm.convertToImage());
  pi.convertDepth(32);
  Buffer2D b(pi.width(), pi.height(), (uns32*)pi.bits());
  buffer.drawBuffer(b,0,0,x,y,pi.width(), pi.height());

  return;

  if (rotate==270) {

    p.rotate(270);

    p.drawText(-y, x+drawWidth()-f.descent()-1, t);

  } else {
 
    if (point_location & Edge) {
      int vd = (drawHeight()>>1) + edge_border;
      int hd = (drawWidth()>>1) + edge_border;
      double fdx = fabs(edge_vec.x());
      double fdy = fabs(edge_vec.y());
      x -= drawWidth()>>1;
      y += drawHeight()>>1;
         // top or bottom intersect
      if (fdy*trans.drawToRealDX(hd) > trans.drawToRealDY(vd)*fdx) {
	x += trans.realToDrawDX(((edge_vec.x()*trans.drawToRealDY(vd))/fdy));
        y += (edge_vec.y()<0) ? vd : -vd;
	// left or right intersect
      } else {
	y -= trans.realToDrawDY(((edge_vec.y()*trans.drawToRealDX(hd))/fdx));
	x += (edge_vec.x()<0) ? -hd : hd;
      }

    } else {
      if (point_location & HCentre) x -= drawWidth()>>1;
      else if (point_location & Right) x -= drawWidth();
      if (point_location & VCentre) y += drawHeight()>>1;
      else if (point_location & Top) y += drawHeight();
    }
    
    x += dx;
    y += dy;
     //if (selected_flag || true) p.drawRect(x, y-drawHeight(), drawWidth(), drawHeight());
    //if (selected_flag) p.drawRect(x, y-drawHeight()+1, drawWidth(), drawHeight()-1);
    p.drawText(x, y-f.descent()-1, t);
  }
  */
}



// Label2D::exportPostscript()

void Label2D::drawEPS(const Trans2D& trans, JTextStream& str) const {
  /*
  int px = trans.realToDrawX(pos.x());
  //int py = trans.realToDrawY_Flip(pos.y());
  int py = trans.realToDrawY(pos.y());
  px += dx;
  py -= dy;
  
  pc.stream() << "/ArialMT findfont\n";
  pc.stream() << "12 scalefont\n";
  pc.stream() << "setfont\n";
  pc.stream() << " " << px << " " << py << " moveto\n";
  pc.stream() << "(" << t.latin1() << ") show\n";
  */
}



// Label2D::pixelPointOn()

bool Label2D::pixelPointOn(const Trans2D&, int tx, int ty) const {
  tx -= x;
  ty -= y;
  return (tx>=0 && ty>=0 && tx<drawWidth() && ty<drawHeight()); 
}

