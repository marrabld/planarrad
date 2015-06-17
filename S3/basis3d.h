
// basis3d.h

#ifndef _JUDE_S3_BASIS3D_H
#define _JUDE_S3_BASIS3D_H

#include "S3/vec3d.h"
#include "base/jtextstream.h"

using namespace jude::base;



//class DumpStreamIn;
//class DumpStreamOut;

namespace jude {
namespace S3 {



class Basis3D {

  Vec3D x_axis;
  Vec3D y_axis;
  Vec3D z_axis;
 
  //Basis3D(DumpStreamIn& str) { read(str); }
 
public:

  static const Basis3D AxesAligned;

  //static void dumpWrite(Basis3D* bas, DumpStreamOut& str);
  //static Basis3D* dumpCreate(DumpStreamIn& str);

  Basis3D() { }
  Basis3D(const Vec3D x, const Vec3D& y, const Vec3D& z) : x_axis(x), y_axis(y), z_axis(z) { }
 
  void write(JDataStream& str) const; 
  void read(JDataStream& str);

  void copy(const Basis3D& b);

  void makeFromNotUnits(const Vec3D& x, const Vec3D& y);
  void makeFromNotUnits(const Vec3D& x, const Vec3D& y, const Vec3D& z);

  const Vec3D& xAxis() const { return x_axis; }
  const Vec3D& yAxis() const { return y_axis; }
  const Vec3D& zAxis() const { return z_axis; }

  void setTo(const Vec3D& vx, const Vec3D& vy, const Vec3D& vz) { x_axis = vx; y_axis = vy; z_axis = vz; }
 
  void setXAxis(const Vec3D& v) { x_axis = v; }
  void setYAxis(const Vec3D& v) { y_axis = v; }
  void setZAxis(const Vec3D& v) { z_axis = v; }

  void rotateAboutZ(double rad);

  void rotateAboutExternalX(double rad);
  void rotateAboutExternalY(double rad);
  void rotateAboutExternalZ(double rad);

  void setFromZDirec(const Vec3D& vz, bool xaxis_in_xy_plane, bool xaxis_in_yz_plane, bool xaxis_in_xz_plane, 
		     bool yaxis_on_positive_side, double rot_deg);

  Vec3D intoBasis(const Vec3D& v) const;
  Vec3D outOfBasis(const Vec3D& v) const;

  friend JDataStream& operator>>(JDataStream& str, Basis3D& rhs);
  friend JDataStream& operator<<(JDataStream& str, const Basis3D& rhs); 

  bool checkOk() const;

  void display(JTextStream& str) const;
};



inline JDataStream& operator>>(JDataStream& str, Basis3D& rhs) { 
  return str >> rhs.x_axis >> rhs.y_axis >> rhs.z_axis;
}



inline JDataStream& operator<<(JDataStream& str, const Basis3D& rhs) { 
  return str << rhs.x_axis << rhs.y_axis << rhs.z_axis;
}



} } // namespace jude::S3

#endif // _BASIS3D_H_JDH

