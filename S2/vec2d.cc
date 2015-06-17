
// vec2d.cc

#include <math.h>
#include <stdio.h>

#include "vec2d.h"
#include "base/jtypes.h"
#include "base/jdatastream.h"
#include "math/rand.h"

using namespace jude::base;
using namespace jude::math;
using namespace jude::S2;



/*!
\brief Returns the length of the vector.
*/

double Vec2D::length() const {
  return sqrt(xp*xp + yp*yp);
}



/*!
\brief Returns the length of the vector squared.

Note it can often be quicker to deal with squared lengths of vectors. For example, when comparing the length of two vectors to determine which is the longest then comparing the sqaure of their lengths is preferred, since it saves the compuation of two square roots.
*/

double Vec2D::lengthSQ() const {
  return xp*xp + yp*yp;
}



/*! \brief Sets the length of the vector to \a len. The direction of the vector from (0,0) is preserved.
  If the vector is (0,0) it is unchanged. */

void Vec2D::setLength(double len) {
  double f = sqrt(xp*xp + yp*yp);
  if (f != 0) f = len / f;;
  xp *= f;
  yp *= f;
}



/*! \brief Sets the length of the vector to 1. The direction of the vector from (0,0) is preserved.
  If the vector is (0,0) it is unchanged. */

void Vec2D::setUnitLength() {
  double f = sqrt(xp*xp + yp*yp);
  if (f != 0) f = 1.0 / f;
  xp *= f;
  yp *= f;
}



// Vec2D::squaredDistance()
/**
Returns the square of the distance of this point from <i>p</i>.
In some cases, for example searching for the closest point <i>p</i>, the square of the distance is sufficient and is more efficient than actual distance as it avoids the sqaure root calculation.
*/

double Vec2D::distanceSQ(const Vec2D& p) const {
  double xv = xp - p.x();
  double yv = yp - p.y();
  return xv*xv + yv*yv;
}



// Vec2D::distance()
/**
Returns the distance of this point from <i>p</i>.
*/

double Vec2D::distance(const Vec2D& p) const {
  double xv = xp - p.x();
  double yv = yp - p.y();
  return sqrt(xv*xv + yv*yv);
}




void Vec2D::read(JDataStream& str) {
  str >> xp >> yp;
}



void Vec2D::write(JDataStream& str) const {
  str << xp << yp;
}



// Vec2D::display()
/*!
\brief Output the x and y values of the vector as text. */

void Vec2D::display(JTextStream& str) const {
  str.printf("x= %f y= %f\n",xp,yp);
}

