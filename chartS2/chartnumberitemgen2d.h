
// chartnumbergen2d.h

#ifndef _JUDE_CHARTS2_CHARTNUMBERGEN2D_H
#define _JUDE_CHARTS2_CHARTNUMBERGEN2D_H

#include "drawS2/rectangle2d.h"
#include "base/jstring.h"
#include "math/rand.h"

using namespace jude::base;
using namespace jude::math;
using namespace jude::drawS2;

namespace jude {
namespace chartS2 {



class ChartNumberGen2D {

public:

  virtual ~ChartNumberGen2D() { }

  virtual int width(const JString& s) const { return 15; }
  virtual int height(const JString& s) const { return 7; }
  virtual int heightHint() const { return 7; }

  virtual DrawItem2D* createItem(const JString& s) { return new Rectangle2D(Box2D(Vec2D(0,0),Vec2D(9+2*rand_int(3),7)), JColour::Blue); }
};



} } // namespace jude::chartS2

#endif // _JUDE_CHARTS2_CHARTNUMBERGEN2D_H

