
// slabintegrator.h

#ifndef _JUDE_RAD_SLAB_H
#define _JUDE_RAD_SLAB_H

#include "rad/rdd_matrixdft.h"

namespace jude {
namespace rad {



class Slab {

public:


  RDD_MatrixDFT mRup;
  RDD_MatrixDFT mTup;
  RDD_MatrixDFT mRdown;
  RDD_MatrixDFT mTdown;

};



} } // namespace jude::rad

#endif // _JUDE_RAD_SLAB_H

