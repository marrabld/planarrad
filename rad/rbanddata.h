
#ifndef _JUDE_RAD_RBANDDATA_H
#define _JUDE_RAD_RBANDDATA_H

#include "rad/radtypes.h"
#include "math/ublas.h"

using namespace boost::numeric::ublas;

namespace jude {
namespace rad {



/*! \brief Radiance-to-radiance function values in a series of bands.*/

class RBandData {

  vector<datatype_r> data;

public:

  RBandData(int n) { data = scalar_vector<datatype_r>(n); }
  RBandData(int n, datatype_s d) { data = scalar_vector<datatype_r>(n, d); }

};



} } // namespace jude::rad

#endif // _JUDE_RAD_RBANDDATA_H

