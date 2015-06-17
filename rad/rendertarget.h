
// rendertarget.h

#ifndef _JUDE_RAD_RENDERTARGET_H
#define _JUDE_RAD_RENDERTARGET_H

#include "math/ublas.h"

namespace jude {
namespace rad {



class RenderTarget {

public:

  virtual ~RenderTarget() { }

  virtual int width() const = 0;
  virtual int height() const = 0;

  virtual void clear() = 0;

  virtual void renderPixel(int x, int y, const vector<float>& v) = 0;
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RENDERTARGET_H

