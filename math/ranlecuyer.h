
// ranlecuyer.h

#ifndef _JUDE_MATH_RANLECUYER_H
#define _JUDE_MATH_RANLECUYER_H

#include "rangen.h"
#include "base/jmutex.h"

namespace jude {
namespace math {

using namespace jude::base;



/*! \brief A 32-bit random number generator based on L'Ecuyer (1988). */

class RanLEcuyer : public RanGen {

  int32 s1;
  int32 s2;

  JMutex mutex;

public:

  RanLEcuyer() : s1(19374), s2(649203) { }

  void initSeed(int32 i); 

  double generateDouble();
};



} } // namespace jude::math

#endif // _JUDE_MATH_RANLECUYER_H

