
// fixedsize.h

#ifndef _JUDE_RAD_FIXEDSIZE_H
#define _JUDE_RAD_FIXEDSIZE_H

#include "radtype.h"

namespace jude {
namespace rad {



/*! \brief Radiance, irradiance or function data in a series of bands, statically sized.

\todo Might have to split into "s" and "r" types.
*/

template <typename A>
class FixedSize {

  static elem_count;

  A* data;

public:

  FixedSize();
  ~FixedSize();

};



} } // namespace jude::rad

#endif // _JUDE_RAD_FIXEDSIZE_H

