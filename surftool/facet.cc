
// facet.cc

#include "facet.h"



bool Facet::intersects(const Vec3D& v0, const Vec3D& dv, double& alpha) const {

  //const double ZERO = 1E-10;

    // calculate position of intersection of ray and polygon plane
  alpha = Vec3D::dotProduct(norm, dv);
    // line and triangle are coplanar
  if (alpha == 0) return false;
    // alpha is multiple of dv from v0
  alpha = -(Vec3D::dotProduct(norm, v0) + D_value) / alpha;
  
  Plucker3D L(v0, v0+dv);

  double s0 = Plucker3D::sideOperator(L, plucker_01);
  double s1 = Plucker3D::sideOperator(L, plucker_12);
  double s2 = Plucker3D::sideOperator(L, plucker_20);
  
  //return ((s0<=ZERO && s1<=ZERO && s2<=ZERO) || (s0>=-ZERO && s1>=-ZERO && s2>=-ZERO));
  return ((s0<0 && s1<0 && s2<0) || (s0>=0 && s1>=0 && s2>=0));
};

