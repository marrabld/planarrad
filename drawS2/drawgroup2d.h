
// drawgroup2d.h

#ifndef _JUDE_DRAWS2_DRAWGROUP2D_H
#define _JUDE_DRAWS2_DRAWGROUP2D_H

#include "base/jlist.h"
#include "drawitem2d.h"

using namespace jude::base;
using namespace jude::S2;

namespace jude {
namespace drawS2 {



// DrawGroup2D

class DrawGroup2D : public DrawItem2D {

protected:

  JList<DrawItem2D> drawing_items;

public:

  DrawGroup2D();

  virtual void addItem(DrawItem2D* item);
  virtual void removeItem(DrawItem2D* item);
  virtual void toTop(DrawItem2D* item);
  virtual void toBottom(DrawItem2D* item);

  void removeAllItems();
  void deleteAllItems();

  JList<DrawItem2D>& itemList() { return drawing_items; }
  const JList<DrawItem2D>& itemList() const { return drawing_items; }

  int itemCount() const;

  virtual const Box2D boundingBox() const;

  virtual void draw(const Trans2D& trans, JBuffer& buff) const;
  virtual void drawEPS(const Trans2D& trans, JTextStream& str) const;
};



} } // namespace jude::drawS2

#endif // _JUDE_DRAWS2_DRAWGROUP2D_H


