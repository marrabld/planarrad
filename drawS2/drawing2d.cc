
// drawing2d.cc

#include "drawing2d.h"
#include "drawS2/trans2d.h"
#include "base/jbuffer.h"
#include <math.h>

using namespace jude::drawS2;



// Drawing2D::(constructor)

Drawing2D::Drawing2D() {
  background_colour = JColour(255,255,255);
  //pixel_border = 0;
  auto_bounding_box = true;
  //ignore_pixel_objects = false;
  //distortable = true;
  //postscript_border = true;
}



// Drawing2D::realBoundingBox()

const Box2D Drawing2D::boundingBox() const {
  if (auto_bounding_box) return top_group.boundingBox();
  else return fixed_bounding_box;
}



// Drawing2D::getTranslation() [virtual]
/*
void Drawing2D::getTranslation(Trans2D& trans) {

  //trans.setXOffset(pixel_border);
  //trans.setYOffset(pixel_border);

    // for auto scaling then base the translation on the drawing bounding box
  if (auto_bounding_box) { 

    printf("Drawing2D::getTranslation() needs fixing\n");
    exit(1);

  } else {
    //trans.setBoundingBox(bounding_box);
    //trans.setXScale((trans.drawWidth() - (pixel_border<<1)) / bounding_box.width());
    //trans.setYScale((trans.drawHeight() - (pixel_border<<1)) / bounding_box.height());
  }
*/
  /*
todo

  trans.setXOffset(pixel_border);
  trans.setYOffset(pixel_border);

    // for auto scaling then base the translation on the drawing bounding box
  if (auto_bounding_box) { 
      // get the basic bounding box of objects positioned in real co-ordinates
    trans.setBoundingBox(top_group.realBoundingBox());
    trans.setXScale((trans.drawWidth()-(pixel_border<<1)) / trans.boundingBox().width());
    trans.setYScale((trans.drawHeight()-(pixel_border<<1)) / trans.boundingBox().height());
      // remove distortion if the drawing should not be stretched
    if (!distortable) trans.removeDistortion();
      // expand the bounding box for pixel sized objects
    trans.initPixelExcess();
    if (!ignore_pixel_objects) top_group.expandForPixelObjects(trans);
      // calculate the scale to draw at to fit the objects in
      // the scale will be too large if any pixel sized objects (e.g. labels) extend outside the bounding box
    trans.incorporatePixelExcess(0);
    trans.setXScale((trans.drawWidth()-(pixel_border<<1)) / trans.boundingBox().width());
    trans.setYScale((trans.drawHeight()-(pixel_border<<1)) / trans.boundingBox().height());
  
    // otherwise the edges of the drawing have been set externally
  } else {
    trans.setBoundingBox(bounding_box);
    trans.setXScale((trans.drawWidth() - (pixel_border<<1)) / bounding_box.width());
    trans.setYScale((trans.drawHeight() - (pixel_border<<1)) / bounding_box.height());
  }
  */
//}



// Drawing2D::moveSelectedByPixel()

//void Drawing2D::moveSelectedByPixel(const Trans2D& trans, int dx, int dy) {
//  top_group.moveByPixel(trans, dx, dy);
//}



// Drawing2D::selectAtPixelPoint()

//void Drawing2D::selectAtPixelPoint(const Trans2D& trans2, int x, int y) {
  //JVec2D p(trans.drawToRealX(x),trans.drawToRealY(y));
  //printf("select at real %f %f\n",p.x(), p.y());
//  top_group.selectAtPixelPoint(trans2,x,y);
//}
 


// Drawing2D::draw()

void Drawing2D::draw(const Trans2D& trans, JBuffer& buff) const {
    // clear the buffer to background colour
  buff.fill(background_colour.rgb());
    // recursively draw items
  top_group.draw(trans, buff);
}



// Drawing2D::exportPostscript()

void Drawing2D::exportEPS(const Trans2D& trans, const JString& filepath) const {

  /*
  JPostscriptContext pc(filepath);

  pc.stream() << "%!PS-Adobe EPSF-3.0\n";
  pc.stream() << "%%Title: " << filepath.latin1() << "\n";
  pc.stream() << "%%Orientation: Portrait" << "\n";
  pc.stream() << "%%Pages: 1" << "\n";
  if (!postscript_border) {
    pc.stream() << "%%BoundingBox: 0 0 " << trans.drawAbsWidth() << " " << trans.drawAbsHeight() << "\n";
    pc.stream() << "%%EndComments\n";
  } else {
    int w = 0;
    pc.stream() << "%%BoundingBox: " << -w << " " << -w << " " << trans.drawAbsWidth()+w << " " << trans.drawAbsHeight()+w << "\n";
    pc.stream() << "%%EndComments\n";
    pc.stream() << "1 setlinewidth\n";
    pc.stream() << "newpath\n";
    pc.stream() << -w << " " << -w << " moveto\n";
    pc.stream() << trans.drawAbsWidth()+w << " " << -w << " lineto\n";
    pc.stream() << trans.drawAbsWidth()+w << " " << trans.drawAbsHeight()+w << " lineto\n";
    pc.stream() << -w << " " << trans.drawAbsHeight()+w << " lineto\n";
    pc.stream() << -w << " " << -w << " lineto\n";
    pc.stream() << "stroke\n";
  }
  top_group.draw(trans,pc);
  */

}

