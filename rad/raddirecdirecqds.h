
// raddirecdirecqds.h

#ifndef _JUDE_RAD_RADDIRECDIRECQDS_H
#define _JUDE_RAD_RADDIRECDIRECQDS_H

#include "ds/quaddatasource.h"

namespace jude {
namespace rad {



/*! \brief Interface for classes that retrieve info to display in text output of a directional data table. */
 
template <typename A> 
class RadDirecDirecQDS : public QuadDataSource {

  const RadDirecDirecImp<A>& rdd;

  int in_index;
  int band_index;

public:

  RadDirecDirecQDS(const RadDirecDirecImp<A>& rdd, int ii, int bi) : 
    rdd(rdd), 
    in_index(ii), 
    band_index(bi) { }

  void displayAtIndex(JTextStream& str, int out_index) const {
    str << rdd.atIndices(in_index, out_index).bandValue(band_index);
  }
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECDIRECQDS_H

