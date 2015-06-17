
// spquad.h

#ifndef _JUDE_DS_SPQUAD_H
#define _JUDE_DS_SPQUAD_H

#include "dsquad.h"
#include "spquadintegrator.h"
#include "S3/vec3d.h"
#include "math/rand.h"

namespace jude {
namespace ds {

using namespace jude::base;
using namespace jude::S3;
using namespace jude::math;



/*! \brief A directional partition quad type defined by angles of constant zenith and azimuth as used in SpherePartionVxH and HemiSpherePartionVxH types. 

  For these quads the solid angle subtended is calculated directly as: [cos(theta_min) - cos(theta_max)] * [phi_max - phi_min]\n 
*/


class SpQuad : public DsQuad {

  Vec3D mean_direc;
  Vec3D point_direc;

  double mean_theta;
  double point_theta;
  double phi_centre;

  double theta_min;
  double theta_max;
  double phi_min;
  double phi_max;

  double mean_cos_theta;
  double solid_angle;

  double mean_cube_distance;
  double point_cube_distance;

  bool is_end_cap;
 
public:

  enum { TOP_CAP, BOTTOM_CAP, QUAD };

  SpQuad() { }
  SpQuad(const SpQuad* q0, const SpQuad* q1, const SpQuad* q2, const SpQuad* q3);

  void setTo(int type, double tmin, double tmax, double pmin, double pmax);

  const Vec3D& meanDirec() const { return mean_direc; }
  const Vec3D& pointDirec() const { return point_direc; }

  double meanTheta() const { return mean_theta; }
  double meanPhi() const { return phi_centre; }

  double pointTheta() const { return point_theta; }
  double pointPhi() const { return phi_centre; }

  //double theta() const { return theta_centre; }
  //double phi() const { return phi_centre; }

  double thetaMin() const { return theta_min; }
  double thetaMax() const { return theta_max; }
  double phiMin() const { return phi_min; }
  double phiMax() const { return phi_max; }

  double meanCosTheta() const { return mean_cos_theta; }
  double solidAngle() const { return solid_angle; }
  double solidAngleAbsCosTheta() const { return solid_angle * fabs(mean_cos_theta); }
  double meanCubeDistance() const { return mean_cube_distance; }
  double pointCubeDistance() const { return point_cube_distance; }

  Vec3D randomDirec() const {
    double t = acos(uniform_dbn(cos(theta_min), cos(theta_max))); // more evenly distributed over quad ?
    // double t = uniform_dbn(theta_min, theta_max);
    double p = uniform_dbn(phi_min, phi_max);
    return Vec3D(t,p);
  }
 
  bool isEndCap() const { return is_end_cap; }

  QuadIntegrator* createIntegrator(double sa=0) const { return new SpQuadIntegrator(*this,sa); }

  bool display(JTextStream& str, int index, int code) const;
};



} } // namespace jude::ds

#endif // _JUDE_DS_SPQUAD_H

