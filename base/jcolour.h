
// jcolour.h

#ifndef _JUDE_BASE_COLOUR_H
#define _JUDE_BASE_COLOUR_H

#include "base/jtypes.h"

namespace jude {
namespace base {



// JColour should be byte-equivalent to QT's QColor
// these macros can be used for conversion

#define JCOLOUR_TO_QCOLOR(jc) QColor(jc.red(), jc.blue(), jc.green(), jc.alpha())
#define QCOLOR_TO_JCOLOUR(qc) JColour(qc.rgba())



/*! \brief Representation of an RGBA colour used when rendering to the screen or images.

This class is intended to hide from the user the details of the RGBA bit mapping into an unsigned 32-bit integeer. 
So that code using JColour will be platform-independent. However currently only one mapping is supported:
R 0xff000000, G 0x00ff0000, B 0x0000ff00, A 0x000000ff.

Several single instantiated static members are provided for easy colour referencing: White, Black, MidGrey, Red, Green, Blue, Yellow.
*/

class JColour {

  uns32 v;

public:

  JColour() : v(0) { }
  JColour(uns32 v) : v(v) { }
  JColour(uns32 r, uns32 g, uns32 b) : v((255<<24) | (r<<16) | (g<<8) | b) { }

  uns32 rgb() const { return v; }
  uns32 rgba() const { return v; }

  uns32 red() const { return (v>>16) & 255; }
  uns32 green() const { return (v>>8) & 255; }
  uns32 blue() const { return v & 255; }
  uns32 alpha() const { return (v>>24) & 255; }

  void setRgb(int r, int g, int b) {  v = (255<<24) | (r<<16) | (g<<8) | b; }

  void saturate(double r, double g, double b);

  static uns8 red(const uns32& v) { return (v>>16) & 255; }
  static uns8 green(const uns32& v) { return (v>>8) & 255; }
  static uns8 blue(const uns32& v) { return v & 255; }
  static uns8 alpha(const uns32& v) { return (v>>24) & 255; }
  
  static const JColour White;
  static const JColour Black;
  static const JColour MidGrey;
  static const JColour Red;
  static const JColour Green;
  static const JColour Blue;
  static const JColour Yellow;
};



} } // namespace jude::base

#endif // _JUDE_BASE_COLOUR_H

