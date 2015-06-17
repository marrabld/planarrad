
// hemispheredraw.h

#ifndef _JUDE_RAD_HEMISPHEREDRAW_H
#define _JUDE_RAD_HEMISPHEREDRAW_H

#include "base/jtypes.h"
#include "rad/raddirecimp.h"
#include "rad/sbanddata.h"

namespace jude { namespace base {
class JBuffer;
} }

using namespace jude::base;

namespace jude {
namespace rad {

class BandSpec;



class HemiSphereDraw {

    // the band specification of the data
  const BandSpec* bs;
    // the spherical data
  const RadDirecImp<SBandData>* rdata;
    // if true then draw the z-plus hemisphere else draw the z-minus one
  bool do_zplus;
    // sensitivity to convert radiance to colour data
  double sens_k;
    // sensitivity for relative sens_k of 1 
  double max_sens_k;

    // directional data converted to rgb colour data
  mutable uns32* rgb_data;

  void recalcRGBData();
 
public:

  HemiSphereDraw();
  ~HemiSphereDraw();
 
    /*! Sets the band specification to use. */
  void setBandSpec(const BandSpec* b) { bs = b; }
    /*! Sets the source data to use. */
  void setSourceData(const RadDirecImp<SBandData>* rd);
    /*! Sets which hemisphere to view. */
  void setViewZPlusHemisphere(bool f) { do_zplus = f; }

    /*! Returns the current sensitivity k-value for converting Lumens to 0-255 RGB elements. */
  double sensK() const { return sens_k; }
    /*! Returns the suggested maximum sensitivity k-value, defaults to 1 but is set after calling autoSetSensK(). */
  double maxSensK() const { return max_sens_k; }
    /*! Sets the current sensitivity k-value as being \a s from 0 to maxSensK(). */
  void setRelativeSensK(double s);
    /*! Sets the current sensitivity k-value for converting Lumens to 0-255 RGB elements. */
  void setSensK2(double s);
    /*! Sets the current sensitivity k-value and maximum automatically from the current source data, and returns the value. */
  double autoSetSensK();

  void drawToBuffer(JBuffer& buff);
};



} } // namsepace jude::rad

#endif // _JUDE_RAD_HEMISPHEREDRAW_H

