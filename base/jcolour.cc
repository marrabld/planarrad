
// jcolour.cc

#include "jcolour.h"

using namespace jude::base;


const JColour JColour::White(255,255,255);
const JColour JColour::Black(0,0,0);
const JColour JColour::MidGrey(127,127,127);
const JColour JColour::Red(255,0,0);
const JColour JColour::Green(0,255,0);
const JColour JColour::Blue(0,0,255);
const JColour JColour::Yellow(0,255,255);



/*!
\brief Linearly scales the supplied RGB parameters \c r, \c g, \c b, such that the largest becomes 255. 
Sets this colour to the resultant RGB values with an alpha value of 255. */

void JColour::saturate(double r, double g, double b) {

  double f = (r>g) ? r : g;
  if (b>f) f = b;
  f = 255 / f;

  setRgb((int) (r * f), (int) (g * f), (int) (b * f));
}

