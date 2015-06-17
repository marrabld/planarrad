
// rangen.h

#ifndef _JUDE_MATH_RANGEN_H
#define _JUDE_MATH_RANGEN_H

#include "base/jtypes.h"

namespace jude {
namespace math {

using namespace jude::base;



/*! \brief Interface for random number generators. */

class RanGen {

public:

  virtual ~RanGen() { }

  virtual void initSeed(int32 i) = 0;
  virtual double generateDouble() = 0;
};


} } // namespace jude::math;



#endif // _JUDE_MATH_RANGEN_H

