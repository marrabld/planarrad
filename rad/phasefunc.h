
// phasefunc.h

#ifndef _JUDE_RAD_PHASEFUNC_H
#define _JUDE_RAD_PHASEFUNC_H

#include "math/ublas.h"
#include "S3/vec3d.h"
#include "file/filesetdesc.h"
#include "rad/sbanddata.h"
#include "rad/bandspec.h"

using namespace jude::base;
using namespace jude::S3;
using namespace jude::math;
using namespace jude::file;

namespace jude {
namespace rad {



/*! Stores a phase function, i.e. probability of scattering dependent on angle. 
As is normal for phase functions the probability is stored per unit solid angle so units are inverse steradians (sr-1). 
Several modes of creating the phase function are provided, including built-in isotropic and Petzold's phase functions. 
Files describing the phase function at different wavelengths can be provided. 

\todo The return of random scattering angles is quantized to (size), that could be a problem in some contexts.
*/

class PhaseFunc {

    // and enum value which describes how to build the phase function
  int mode;
    // number of files used for initialisiation (zero if none)
  int file_count;
    // pointer to band specification of null if none
  const BandSpec* bs;
    // this is the resolution to store the probability lookup
  int size;

    // the integral of the phase function over solid angle = angular pdf, one for each band
  vector<double>* value_pdf;
    // the probability lookup - values are the angle of scattering as can be used in a monte-carlo model 
  vector<double>* value_sel;
    // array of wavelengtsh in nm that the above bands correspond to
  double* wave_len;

  vector<double>* value_sel_ipol;
  vector<double>* value_reg_ipol;
 
    // number of regular angle intervals for lookup table, e.g. 1801
  int reg_size;
    // angle lookup table for phase func (units sr-1) with values at regular angle intervals
  vector<double>* value_reg;
 
  bool is_random;

  bool init_ok;

  void build(int band, vector<double>& angle, vector<double>& value);
  void buildAngleLookup(vector<double>& targ_value, const vector<double>& src_angle, const vector<double>& src_value);

  double scatterAngleFromSingle() const;
  double scatterAngleFromBand(int band) const;
  double scatterAngleFromIpol(int band) const;
  double scatterAngleFromWavelen(double wavelen) const;

  double valueFromAngleSingle(double ang_rad) const;
  double valueFromAngleBand(int band, double ang_rad) const;
  double valueFromAngleIpol(int band, double ang_rad) const;
  double valueFromAngleWavelen(double w, double ang_rad) const;

  SBandData spectraFromAngleSingle(double ang_rad) const;
  SBandData spectraFromAngleBand(double ang_rad) const;
  SBandData spectraFromAngleIpol(double ang_rad) const;
  SBandData spectraFromAngleWavelen(double ang_rad) const;

  Vec3D scatterDirecFromAngle(double angle, const Vec3D& dv) const;
 
  bool readFromTextFile(int file_num, vector<double>& angle, vector<double>& value, const FileDesc& fd);
  //bool readFournierForand(int file_num, vector<double>& angle, vector<double>& value, const FileDesc& fd);

  bool buildFournierForand(double Bfrac, vector<double>& angle, vector<double>& value);
  void buildPureWater(vector<double>& angle, vector<double>& value);

  double calcBp(double mu);
  double ffValue(double ang_rad, double ff_mu);

public:

  enum { RANDOM_MODE, SINGLE_MODE, BAND_MODE, IPOL_MODE, WAVE_MODE };

  PhaseFunc(const JString& mode_str, const FileSetDesc& fs, const std::vector<double>& wl, const BandSpec* bs);
  ~PhaseFunc();

  bool initOk() const { return init_ok; } 

  Vec3D scatterDirec(int band, const Vec3D& dv) const;

    /*! \brief Returns the value of the phase function for the scattering angle \a ang_rad in band \a band. */
  double valueFromAngle(int band, double ang_rad) const;
    /*! \brief Returns the value of the phase function for the scattering angle \a ang_rad in each band. */
  SBandData spectralValuesFromAngle(double ang_rad) const;
};



} } // namespace jude::rad

#endif // _JUDE_RAD_PHASEFUNC_H


