
// facet.h

#ifndef _JUDETOOLS_FACET_H
#define _JUDETOOLS_FACET_H

#include "S3/vec3d.h"
#include "S3/plucker3d.h"
#include "base/jlog.h"

using namespace jude::S3;
using namespace jude::base;



class Facet {

  int v0;
  int v1;
  int v2;

  Plucker3D plucker_01;
  Plucker3D plucker_12;
  Plucker3D plucker_20;

  Vec3D norm;
  double D_value;

public:

  int vertexIndex0() const { return v0; }
  int vertexIndex1() const { return v1; }
  int vertexIndex2() const { return v2; }

  void setTo(int i0, int i1, int i2) { v0=i0; v1=i1; v2=i2; }

  void calcGeometry(const Vec3D* vec_array) {
    plucker_01.setTo(vec_array[v0], vec_array[v1]);
    plucker_12.setTo(vec_array[v1], vec_array[v2]);
    plucker_20.setTo(vec_array[v2], vec_array[v0]);
    norm = Vec3D::crossProduct(vec_array[v1]-vec_array[v0], vec_array[v0]-vec_array[v2]);
    norm.setUnitLength();
    D_value = -Vec3D::dotProduct(norm,  vec_array[v0]);
    //jlog::ls << "norm: " << norm << " D : " << D_value << "\n";
  }

  const Vec3D& surfaceNormal() const { return norm; }

  bool intersects(const Vec3D& v0, const Vec3D& dv, double& alpha) const;
};



#endif // _JUDETOOLS_FACET_H

