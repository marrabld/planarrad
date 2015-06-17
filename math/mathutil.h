
// mathutil.h

#ifndef _JUDE_MATH_MATHUTIL_H
#define _JUDE_MATH_MATHUTIL_H

#include <math.h>

namespace jude {
namespace math {



/*! \brief Convert degrees to radians. */

static inline double degToRad(double d) { return (M_PI * d) / 180.0; }



/*! \brief Convert radians to degrees. */

static inline double radToDeg(double r) { return (180.0 * r) / M_PI; }



/*! \brief Return the fractional component of \a v. */

template <typename T>
static double frac(T v) { return (T) v - floor(v); }



} } // namespace jude::math

#endif // _JUDE_MATH_MATHUTIL_H

