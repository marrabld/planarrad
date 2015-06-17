
// spquadintegrator.h

#ifndef _JUDE_DS_SPQUADINTEGRATOR_H
#define _JUDE_DS_SPQUADINTEGRATOR_H

#include "ds/quadintegrator.h"
#include "S3/vec3d.h"

namespace jude {
namespace ds {

using namespace jude::S3;

class SpQuad;



class SpQuadIntegrator : public QuadIntegrator {

  const SpQuad& q;
 
  int num_ct;
  int num_phi;

  int curr_ct;
  int curr_phi;

  double ct_start;
  double ct_d;

  double phi_start;
  double phi_d;

  Vec3D curr_direc;

  double curr_cos_theta;
  double curr_sa;
 
  void setDirection();

public:

  SpQuadIntegrator(const SpQuad& q, double sa=0);

  bool atEnd() const { return (curr_phi>=num_phi); }

  void setSubSolidAngle(double sa);

  int subQuadCount() const { return num_ct * num_phi; }

  void reset();
  void next();

  const Vec3D& direction() const { return curr_direc; }

  double cosTheta() const { return fabs(curr_cos_theta); }
  double solidAngle() const { return curr_sa; }
  double solidAngleAbsCosTheta() const { return curr_sa * fabs(curr_cos_theta); }
};



} } // namespace jude::ds

#endif // _JUDE_DS_SPQUADINTEGRATOR_H

