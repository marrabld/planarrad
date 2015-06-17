
// skytool.h

#ifndef _JUDETOOLS_SKYTOOL_H
#define _JUDETOOLS_SKYTOOL_H

#include "tool/tool.h"
#include "S3/vec3d.h"
#include "rad/sbanddata.h"

namespace jude { namespace rad { class BandSpec; } }
namespace jude { namespace ds { class DirecSpec; } }

using namespace jude::tool;
using namespace jude::S3;
using namespace jude::rad;
using namespace jude::ds;



class SkyTool : public Tool {
 
  BandSpec* master_bs;
  DirecSpec* master_ds_spec;
 
  RadDirecImp<SBandData>* emis; // where the resultant data will be

  //RadDirecImp<SBandDataPol>* emis_pol; // where the resultant data will be

  void init();

  bool getSunPosition(const JParser& prm, double& zenith, double& azimuth, Vec3D& sun_direc);
  bool getTotalAndDiffuseIrrad(const JParser& prm, SBandData& total_irrad, SBandData& diffuse_irrad, SBandData& direct_irrad);

  bool diffuseSky(RadDirecImp<SBandData>& emis, const JParser& prm);
  bool thetaZeroSky(RadDirecImp<SBandData>& emis, const JParser& prm);
  bool specularSky(RadDirecImp<SBandData>& emis, const JParser& prm);
  bool hlidealSky(RadDirecImp<SBandData>& emis, const JParser& prm);
  bool grantSky(RadDirecImp<SBandData>& emis, const JParser& prm);

  bool loadFile(RadDirecImp<SBandData>*& emis, const JParser& prm);

  //bool cieSky(RadDirecImp<SBandData>& emis, const JParser& prm);
  //bool libradtranSky(RadDirecImp<SBandData>& emis, const JParser& prm);

  //double phi_func(double a, double b, double z);
  //double f_func(double c, double d, double e, double chi);
 
public:

  SkyTool();
  ~SkyTool();

  const JString name() const { return "skytool"; }

  const RadDirecImp<SBandData>* fullSpecResult() const { return emis; }

  bool run(const JParser& prm);

  void helpText(JTextStream& str) const;
};



#endif // _JUDETOOLS_SKYTOOL_H

