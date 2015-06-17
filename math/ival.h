
// ival.h

#ifndef _JUDE_MATH_IVAL_H
#define _JUDE_MATH_IVAL_H

namespace jude {
namespace math {



template <typename T>
class ival {

  T lo;
  T hi;

public:

  ival(const T& lo, const T& hi) : lo(lo), hi(hi) { }

};



} } // namespace jude::math

#endif // _JUDE_MATH_IVAL_H

