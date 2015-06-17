
// radutils.h

#ifndef _JUDE_RAD_RADUTILS_H
#define _JUDE_RAD_RADUTILS_H

#include "rad/sbanddata.h"
#include "rad/bandspec.h"
#include "rad/raddirecimp.h"
#include "rad/raddirecdirecimp.h"
#include "base/jtextstream.h"
#include "base/jtexttable.h"

using namespace jude::base;

namespace jude {
namespace rad {



class RadUtils {

  template <typename OBJ>
  static bool handleResample(OBJ& obj, const JParser& prm, const JString& param_code, const JString& desc, const BandSpec& targ_bs);

public:

  static inline int radianceToRGB(double r, const double& sens_k) { return (int) (255 * (1 - exp(-sens_k * r))); }

  static inline double sensToGiveRGBFromRad(double rad, double rgb) { return - log(1 - rgb/255) / rad; }

  static uns32 radianceToRGB(const SBandData& sd, const BandSpec& bs, double sens_k);

  static uns32 radianceToRGB(const SBandData& sd, const SBandData& mean, 
			     const SBandData& rf, const SBandData& gf, const SBandData& bf, double sens_k);

  static bool loadSBandData(SBandData& sd, const JParser& prm, const JString& name, const JString& msg, const BandSpec& master_bs);
  static bool loadRadDirec(RadDirecImp<SBandData>*& rd, const JParser& prm, const JString& param_code, const JString& desc, const DirecStruct& targ_ds, const BandSpec& targ_bs);

  static bool savePlanarIrradiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm);
  static bool saveScalarIrradiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm);
  static bool saveRadiance(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const JParser& prm, const JString& notes_text);
  static bool saveHemisphereImage(const JString& prefix, const JString& postfix, const RadDirecImp<SBandData>& Ldata, const BandSpec& bs, const JParser& prm);

  static bool checkDirecStruct(const JString& fp, const DirecStruct& fp_ds, const DirecStruct& ref_ds);

  static void fillFromHLTable(RadDirecImp<SBandData>& rd, int band, const JTextTable& ttab);

  static void displayHLBlockStyle(const RadDirecImp<SBandData>& rd, int band, JTextStream& str);

  static const SBandData meanCosine(const RadDirecImp<SBandData>& rd);

  static void scale(RadDirecImp<SBandData>& rd, double scale);
  static void makeAzimuthallyAveraged(RadDirecImp<SBandData>& rd);
  static void makeAzimuthallyAveraged(RadDirecDirecImp<SBandData>& rdd);

  static void makeSymmetricallyAveragedAroundPhi(RadDirecDirecImp<SBandData>& rdd, double phi_sym);

  static void reciprocalInverse(RadDirecDirecImp<SBandData>& rdd);

  static void displayInfo(const RadDirecImp<SBandData>& rd, int ds_table_code, JTextStream& str);
  static void displayInfo(const RadDirecDirecImp<SBandData>& rdd, int ds_table_code, JTextStream& str);
  static void displayHalfFunctionsAsReflectance(const RadDirecDirecImp<SBandData>& rdd, JTextStream& str);
  static void displayHalfFunctionsAsScatter(const RadDirecDirecImp<SBandData>& rdd, JTextStream& str);
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RADUTILS_H

