
// plucker3d.h

#ifndef _JUDE_S3_PLUCKER3D_H
#define _JUDE_S3_PLUCKER3D_H

#include "base/jdatastream.h"
#include "S3/vec3d.h"

namespace jude {
namespace S3 {



class Plucker3D {

  double a0;
  double a1;
  double a2;
  double a3;
  double a4;
  double a5;

public:

  Plucker3D() { }

  Plucker3D(const Vec3D& from, const Vec3D& to) : 
    a0(from.x()*to.y() - to.x()*from.y()),
    a1(from.x()*to.z() - to.x()*from.z()),
    a2(from.x() - to.x()),
    a3(from.y()*to.z() - to.y()*from.z()),
    a4(from.z() - to.z()),
    a5(to.y() - from.y()) { }


  void setTo(const Vec3D& from, const Vec3D& to) { 
    a0 = from.x()*to.y() - to.x()*from.y();
    a1 = from.x()*to.z() - to.x()*from.z();
    a2 = from.x() - to.x();
    a3 = from.y()*to.z() - to.y()*from.z();
    a4 = from.z() - to.z();
    a5 = to.y() - from.y();
  }

  static double sideOperator(const Plucker3D& a, const Plucker3D& b) {
    return a.a0*b.a4 + a.a1*b.a5 + a.a2*b.a3 + a.a3*b.a2 + a.a4*b.a0 + a.a5*b.a1;
  }

  const Plucker3D& operator=(const Plucker3D& p) {
    a0 = p.a0;
    a1 = p.a1;
    a2 = p.a2;
    a3 = p.a3;
    a4 = p.a4;
    a5 = p.a5;
    return *this;
  }

  bool read(JDataStream& str, uns32 saved_version) {
    str >> a0 >> a1 >> a2 >> a3 >> a4 >> a5;
    return true;
  }

  bool write(JDataStream& str) const {
    str << a0 << a1 << a2 << a3 << a4 << a5;
    return true;
  }
};



} } // namespace jude::S3

#endif // _JUDE_S3_PLUCKER3D_H

