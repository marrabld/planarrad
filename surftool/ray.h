

// ray.h

#ifndef _JUDETOOLS_RAY_H
#define _JUDETOOLS_RAY_H

#include "S3/vec3d.h"

class Facet;



class Ray {

  Vec3D v;
  Vec3D dv;

  double pwr;

  bool on_side0;

  const Facet* orig_facet;

public:

 Ray(const Vec3D& v, const Vec3D& dv, double pwr, bool on_side0, const Facet* orig_facet) :
    v(v),
    dv(dv),
    pwr(pwr),
    on_side0(on_side0),
    orig_facet(orig_facet) { }

  const Vec3D& start() const { return v; }
  const Vec3D& direc() const { return dv; }

  double power() const { return pwr; }

  bool onSide0() const { return on_side0; }

  const Facet* origFacet() const { return orig_facet; }
};



#endif // _JUDETOOLS_RAY_H

