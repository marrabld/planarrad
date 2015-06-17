
#ifndef _JUDE_RAD_RADTYPES_H
#define _JUDE_RAD_RADTYPES_H

#include "base/jtypes.h"
#include "math/ival.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace rad {


#ifdef RAD_USE_FLOAT32
typedef float32 basic_radtype;
#else
typedef double64 basic_radtype;
#endif


typedef basic_radtype radtype;



// if radiances and irradiances are to be calculated with interval values
//#ifdef RAD_USE_INTERVAL
//#else
//#endif

typedef basic_radtype radtype;


// datatype_s is the type used for radiance or irradiance data
// datatype_r is the type used for reflectance of scattering functions


//typedef radtype datatype_s;
//typedef radtype datatype_r;



// The following is to give an idea of the strcuture of radinace and irradiance data storage.
// The actual classes are defined elsewhere.


// radiance or irradiance data (single and single-to-single function) in a series of wavelength bands
// these are generally-sizeable, the number of bands is stored in each object.

// class SBandData {
//   ublas::vector<datatype_s> data;
// };

// class RBandData {
//   ublas::vector<datatype_r> data;
// };



// radiance or irradiance data (single and single-to-single function) in a series of wavelength bands
// all objects can only be one size at any given time. The size of FixedSize is statically set, or
// may be fixed at compile time.
// The advantage of these is that each object therefore takes less memory to store and processing 
// also may be faster.

// typedef FixedSize<datatype_s> SFixedBandData;
// typedef FixedSize<datatype_r> RFixedBandData;



// a RadDirecTable is a DirecTable that expects its stored data type to provide some
// radiance or irradiance related functions

// typedef RadDirecTable< SBandData, HemiCubeN > SBandDataHC;
// typedef RadDirecTable< SBandData, CubeN > SBandDataC;



} } // namespace jude::rad

#endif // _JUDE_RAD_RADTYPES_H

