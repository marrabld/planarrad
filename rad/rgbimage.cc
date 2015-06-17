
// rgbimage.cc

#include "rad/rgbimage.h"
#include "rad/radutils.h"

using namespace jude::rad;



void RgbImage::setClosestRGB(const BandSpec& bs) {
  red_band = bs.closestBand(600);
  green_band = bs.closestBand(550);
  blue_band = bs.closestBand(450);
  if (jlog::vbLevel(5)) jlog::ls << "Closest RGB bands: " << red_band << " " << green_band << " " << blue_band << "\n";
}



// RgbImage::clear()

void RgbImage::clear() {
  buff.fill(JColour::MidGrey);
}



void RgbImage::clear(const JColour& c) {
  buff.fill(c);
}



// RgbImage::renderPixel()

void RgbImage::renderPixel(int x, int y, const vector<float>& v) {

  int r,g,b;

  if (use_mult) {
 
    r = (red_band >= 0) ? (int)(fabs(v(red_band)) * red_mult) : 0;
    g = (green_band >= 0) ? (int)(fabs(v(green_band)) * green_mult) : 0;
    b = (blue_band >= 0) ? (int)(fabs(v(blue_band)) * blue_mult) : 0;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

  } else {

    r = (red_band >= 0) ? RadUtils::radianceToRGB(fabs(v(red_band)), red_sens_k) : 0;
    g = (green_band >= 0) ? RadUtils::radianceToRGB(fabs(v(green_band)), green_sens_k) : 0;
    b = (blue_band >= 0) ? RadUtils::radianceToRGB(fabs(v(blue_band)), blue_sens_k) : 0;
  }

  uns32 c = 0xff000000 | (r<<16) | (g<<8) | b;
    
  buff.data()[x + y*width()] = c;
}



// RgbImage::renderPixel()

void RgbImage::renderPixel(int x, int y, const JColour& col) {    
  buff.data()[x + y*width()] = 0xff000000 | col.rgba();
}

