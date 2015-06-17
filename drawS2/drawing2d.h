
// drawing2d.h

#ifndef _JUDE_DRAWS2_DRAWING2D_H
#define _JUDE_DRAWS2_DRAWING2D_H

#include "base/jcolour.h"
#include "base/jstring.h"
#include "S2/box2d.h"
#include "drawgroup2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



// Drawing2D

class Drawing2D {

  DrawGroup2D top_group;
  JColour background_colour;

  bool auto_bounding_box;
  Box2D fixed_bounding_box;


  //int pixel_border;
  //bool distortable;
  //bool ignore_pixel_objects;
  //bool postscript_border;

public:

  Drawing2D();
  virtual ~Drawing2D() { }

  void setBackgroundColour(const JColour& c) { background_colour = c; }
  void setAutoBoundingBox(bool f) { auto_bounding_box = f; }

  //void setPixelBorder(int b) { pixel_border = b; }
  //void setIgnorePixelObjects(bool f) { ignore_pixel_objects = f; }
  //void setDistortable(bool f) { distortable = f; }
  //void setPostscriptBorder(bool f) { postscript_border = f; }

  void setBoundingBox(const Box2D& box) { 
    fixed_bounding_box = box;
    auto_bounding_box = false;
  }
 
  const Box2D boundingBox() const;
 
  void addItem(DrawItem2D* item) { top_group.addItem(item); }
  void removeItem(DrawItem2D* item) { top_group.removeItem(item); }
  void toTop(DrawItem2D* item) { top_group.toTop(item); }
  void toBottom(DrawItem2D* item) { top_group.toBottom(item); }

  void removeAllItems() { top_group.removeAllItems(); }
  void deleteAllItems() { top_group.deleteAllItems(); }
 
  //void moveSelectedByPixel(const Trans2D& trans, int dx, int dy);
  //void selectAtPixelPoint(const Trans2D& trans, int x, int y);

  //virtual void getTranslation(Trans2D& trans);

  virtual void draw(const Trans2D& trans, JBuffer& buffer) const;
  void exportEPS(const Trans2D& trans, const JString& filename) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_DRAWING2D_H


