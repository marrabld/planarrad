
// rgbimage.h

#ifndef _JUDE_RAD_RGBIMAGE_H
#define _JUDE_RAD_RGBIMAGE_H

#include "base/jbuffer.h"
#include "rad/rendertarget.h"
#include "rad/bandspec.h"

using namespace jude::base;

namespace jude {
namespace rad {



// single sided Lambertian reflective polygons

class RgbImage : public RenderTarget {

  JBuffer buff;

  double red_sens_k;
  double green_sens_k;
  double blue_sens_k;

  double red_mult;
  double green_mult;
  double blue_mult;

  bool use_mult;

  int red_band;
  int green_band; 
  int blue_band;

public:

  RgbImage(int xs, int ys) : 
    buff(xs,ys), 
    red_sens_k(1),
    green_sens_k(1),
    blue_sens_k(1),
    red_mult(1),
    green_mult(1),
    blue_mult(1),
    use_mult(false),
    red_band(0),
    green_band(1),
    blue_band(2) { }

  const JBuffer& buffer() const { return buff; }

  int width() const { return buff.width(); }
  int height() const { return buff.height(); }

  void setSensK(double s) { 
    red_sens_k = s; 
    green_sens_k = s; 
    blue_sens_k = s;
    use_mult = false; 
  }

  void setSensK(double rs, double gs, double bs) { 
    red_sens_k = rs; 
    green_sens_k = gs; 
    blue_sens_k = bs;
    use_mult = false; 
  }

  void setMultiplier(double rm, double gm, double bm) { 
    red_mult = rm; 
    green_mult = gm; 
    blue_mult = bm;
    use_mult = true; 
  }

  void setRGBBands(int rb, int gb, int bb) { 
    red_band = rb;
    green_band = gb;
    blue_band = bb;
  }

  void setClosestRGB(const BandSpec& bs); 

  int redBand() const { return red_band; }
  int greenBand() const { return green_band; }
  int blueBand() const { return blue_band; }

  void clear();
  void clear(const JColour& c);

  void renderPixel(int x, int y, const vector<float>& v);
  void renderPixel(int x, int y, const JColour& col);
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RGBIMAGE_H

