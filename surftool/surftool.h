
// surftool.h

#ifndef _JUDETOOLS_SURFTOOL_H
#define _JUDETOOLS_SURFTOOL_H

#include "tool/tool.h"
#include "S3/vec3d.h"
#include "rad/sbanddata.h"

namespace jude { namespace rad { class BandSpec; } }
namespace jude { namespace ds { class DirecSpec; } }

using namespace jude::tool;
using namespace jude::S3;
using namespace jude::rad;
using namespace jude::ds;

class CoxSurface;



class SurfTool : public Tool {
 
  BandSpec* master_bs;
  DirecSpec* master_ds_spec;
 
  int band_count;   // for convienience this is set up from master_bs

  double sub_quad_sa;

  RadDirecDirecImp<SBandData>* func;
  RadDirecDirecImp<SBandData>* func_se;
 
    // required for some interface surfaces
  double refrac_index_0;
  double refrac_index_1;

    // for cox and munk surface
  double targ_size;
  int rays_per_quad;
  int surface_count;
  double min_pwr_frac;

  bool buildAbsentInterface();
  bool buildMirrorInterface();
  bool buildFlatInterface(const JParser& prm);
  bool buildCoxMunkInterface(const JParser& prm);
  bool buildFromFile(const JParser& prm);
 
  bool getRefractiveIndices(const JParser& prm);

  bool doCoxSurface(CoxSurface& cs, RadDirecDirecImp<SBandData>& func_targ, int surf_num, int surf_count);

  void rayCalc(double n0, double n1, 
	       const Vec3D& in_direc, const Vec3D& norm, 
	       Vec3D& trans_direc, Vec3D& reflec_direc,
	       double& trans_power, double& reflec_power);

public:

  SurfTool();
  ~SurfTool();

  const JString name() const { return "surftool"; }

  bool usesRand() const { return true; }

  bool run(const JParser& prm);

  void helpText(JTextStream& str) const;
};



#endif // _JUDETOOLS_SURFTOOL_H

