
// vec3d.h

#ifndef _JUDE_3D_VEC3D_H
#define _JUDE_3D_VEC3D_H

#include <math.h>
#include "base/jdatastream.h"
#include "base/jtextstream.h"
#include "base/jtypes.h"

namespace jude {
namespace S3 {

class LineEqn3D;

using namespace jude::base;



// Vec3D

class Vec3D {

protected:

  double xp;
  double yp;
  double zp;

public:

    /*! \brief Construct without initialisation. Contents are undefined. */
  Vec3D() : xp(0), yp(0), zp(0) { }

    /*! \brief Construct with supplied x,y,z values. */
  Vec3D(double ix, double iy, double iz) : xp(ix), yp(iy), zp(iz) { }

    /*! \brief Construct by reading z,y,z bvalues from a JDataStream. */
  Vec3D(JDataStream& str) {
    str >> xp;
    str >> yp;
    str >> zp; 
  }

  Vec3D(double theta, double phi) :
    xp(sin(theta) * cos(phi)),
    yp(sin(theta) * sin(phi)),
    zp(cos(theta)) { }

  Vec3D& operator=(const Vec3D& rhs) { 
    xp = rhs.xp;
    yp = rhs.yp;
    zp = rhs.zp;
    return *this;
  }
  
  Vec3D& operator+=(const Vec3D& rhs) { 
    xp += rhs.xp;
    yp += rhs.yp;
    zp += rhs.zp;
    return *this;
  }

  Vec3D& operator-=(const Vec3D& rhs) { 
    xp -= rhs.xp;
    yp -= rhs.yp;
    zp -= rhs.zp;
    return *this;
  }

  Vec3D& operator*=(double m) { 
    xp *= m;
    yp *= m;
    zp *= m;
    return *this;
  }
  
  double x() const { return xp; }
  double y() const { return yp; }
  double z() const { return zp; }
  void setX(double ix) { xp = ix; }
  void setY(double iy) { yp = iy; }
  void setZ(double iz) { zp = iz; }

  void makeZero() { xp = yp = zp = 0; }

  void setTo(double ix, double iy, double iz) { xp=ix; yp=iy; zp=iz; }
  void moveBy(double dx, double dy, double dz) { xp+=dx; yp+=dy; zp+=dz; }

  void negate() { 
    xp = -xp; 
    yp = -yp; 
    zp = -zp; 
  }

  void add(const Vec3D& p) { 
    xp += p.xp; 
    yp += p.yp; 
    zp += p.zp; 
  }

  void add(const Vec3D& p0, const Vec3D& p1) { 
    xp = p0.xp + p1.xp; 
    yp = p0.yp + p1.yp; 
    zp = p0.zp + p1.zp; 
  }

  void addMultiple(const Vec3D& p, double m) { 
    xp += m*p.xp; 
    yp += m*p.yp; 
    zp += m*p.zp; 
  }

  void addMultiple(const Vec3D& p0, const Vec3D& p1, double m) { 
    xp = p0.xp + m*p1.xp; 
    yp = p0.yp + m*p1.yp; 
    zp = p0.zp + m*p1.zp; 
  }

  void addMultiples(const Vec3D& p0, double m0, const Vec3D& p1, double m1) { 
    xp = m0*p0.xp + m1*p1.xp; 
    yp = m0*p0.yp + m1*p1.yp; 
    zp = m0*p0.zp + m1*p1.zp; 
  }

  void subtract(const Vec3D& p) { 
    xp -= p.xp; 
    yp -= p.yp; 
    zp -= p.zp; 
  }

  void subtract(const Vec3D& p0, const Vec3D& p1) { 
    xp = p0.xp - p1.xp; 
    yp = p0.yp - p1.yp; 
    zp = p0.zp - p1.zp; 
  }

  void makeMidPoint(const Vec3D& p0, const Vec3D& p1) { 
    xp = (p0.xp+p1.xp) / 2; 
    yp = (p0.yp+p1.yp) / 2; 
    zp = (p0.zp+p1.zp) / 2; 
  }

  static Vec3D midPoint(const Vec3D& p0, const Vec3D& p1) { 
    return Vec3D((p0.xp+p1.xp) / 2, (p0.yp+p1.yp) / 2, (p0.zp+p1.zp) / 2);
  }

  void multiply(double v) { 
    xp *= v; 
    yp *= v; 
    zp *= v; 
  }

  void divide(double v) { 
    xp /= v; 
    yp /= v; 
    zp /= v; 
  }

  Vec3D reverse() const {
    return Vec3D(-xp,-yp,-zp);
  }

  static void rotate(double rad, Vec3D& v0, Vec3D& v1) {
    double n = v0.xp*cos(rad) + v1.xp*sin(rad);
    v1.xp = v1.xp*cos(rad) - v0.xp*sin(rad);
    v0.xp = n;
    n = v0.yp*cos(rad) + v1.yp*sin(rad);
    v1.yp = v1.yp*cos(rad) - v0.yp*sin(rad);
    v0.yp = n;
    n = v0.zp*cos(rad) + v1.zp*sin(rad);
    v1.zp = v1.zp*cos(rad) - v0.zp*sin(rad);
    v0.zp = n;
  }

  void rotateInPlane(double rad, const Vec3D& axis0, const Vec3D& axis1) {
    double xp = dotProduct(*this, axis0);
    double yp = dotProduct(*this, axis1);
    double xp2 = xp*cos(rad) - yp*sin(rad);
    yp = xp*sin(rad) + yp*cos(rad);
    *this = axis0 * xp2;
    add(axis1 * yp);
  }

  void makeNormal(const Vec3D& v0, const Vec3D& v1);

  void makeUnit(double theta, double phi);
  double thetaFromUnit() const;
  double phiFromUnit() const;

  void makePointAlongVector(double m, const Vec3D& p0, const Vec3D& p1);

  // depreceated
  // double dotProduct(const Vec3D& v) const { return xp*v.xp + yp*v.yp + zp*v.zp; }

  static inline double dotProduct(const Vec3D& lhs, const Vec3D& rhs);

  static inline const Vec3D crossProduct(const Vec3D& lhs, const Vec3D& rhs);
 
  //static Vec3D crossProduct(const Vec3D& v0, const Vec3D& v1) {
  //  return Vec3D( v0.y()*v1.z() - v1.y()*v0.z(),
  // 		  v1.x()*v0.z() - v0.x()*v1.z(),
  //		  v0.x()*v1.y() - v1.x()*v0.y() ); 
  //}
 

  static const Vec3D minPoint(const Vec3D& lhs, const Vec3D& rhs) { 
    return Vec3D(min(lhs.x(), rhs.x()), min(lhs.y(), rhs.y()), min(lhs.z(), rhs.z())); 
  }

  static const Vec3D maxPoint(const Vec3D& lhs, const Vec3D& rhs) {
    return Vec3D(max(lhs.x(), rhs.x()), max(lhs.y(), rhs.y()), max(lhs.z(), rhs.z())); 
  }
 
  bool allLessThan(const Vec3D& rhs) const { return (xp<rhs.xp && yp<rhs.yp && zp<rhs.zp); }
  bool allGreaterThan(const Vec3D& rhs) const { return (xp>rhs.xp && yp>rhs.yp && zp>rhs.zp); }
 
  bool allLessThanOrEqual(const Vec3D& rhs) const { return (xp<=rhs.xp && yp<=rhs.yp && zp<=rhs.zp); }
  bool allGreaterThanOrEqual(const Vec3D& rhs) const { return (xp>=rhs.xp && yp>=rhs.yp && zp>=rhs.zp); }
 
  double length() const;
  double lengthSquared() const { return lengthSQ(); }
  double lengthSQ() const;
  void setLength(double len);
  void setUnitLength();
  double squaredDistance(const Vec3D& p) const { return distanceSQ(p); }
  double distanceSQ(const Vec3D& p) const;
  double distance(const Vec3D& p) const;
  //double distanceFromLine(const Vec3D& s, const Vec3D& dv) const;
  double distanceFromLine(const LineEqn3D& line_eqn) const { Vec3D temp; return distanceFromLine(line_eqn, temp); }
  double distanceFromLine(const LineEqn3D& line_eqn, Vec3D& closest_point) const;
  double distanceFromLine(const LineEqn3D& line_eqn, double& t, Vec3D& closest_point) const;
  double angle(const Vec3D& p) const;

  static double distance(const Vec3D& p1, const Vec3D& p2) { return p1.distance(p2); }
  static bool minDistanceLineSegments(const Vec3D& p1, const Vec3D& p2, const Vec3D& p3, const Vec3D& p4, double& mu1, double& mu2);

  void rotateXY(double angle);
  void rotateXZ(double angle);
  void rotateYZ(double angle);

  Vec3D rotate180AboutX() const { return Vec3D(xp,-yp,-zp); }

  static Vec3D hemisphereRand();
  static Vec3D sphereRand();
  static Vec3D hemisphereRand(const Vec3D& axis);

  const Vec3D unitVectorAtRightAngles() const; 

  friend inline const Vec3D operator-(const Vec3D& rhs);
 
  friend inline const Vec3D operator+(const Vec3D& lhs, const Vec3D& rhs);
  friend inline const Vec3D operator-(const Vec3D& lhs, const Vec3D& rhs);

  friend inline const Vec3D operator*(const Vec3D& lhs, const double& rhs);
  friend inline const Vec3D operator/(const Vec3D& lhs, const double& rhs);

  friend inline bool operator==(const Vec3D& lhs, const Vec3D& rhs);
  friend inline bool operator!=(const Vec3D& lhs, const Vec3D& rhs);
 
  friend JDataStream& operator>>(JDataStream& str, Vec3D& rhs);
  friend JDataStream& operator<<(JDataStream& str, const Vec3D& rhs);
  friend JTextStream& operator<<(JTextStream& str, const Vec3D& rhs);
 
  void read(JDataStream& str);
  void write(JDataStream& str) const;

  void display() const;
};



inline const Vec3D operator-(const Vec3D& rhs) { 
  return Vec3D(-rhs.xp, -rhs.yp, -rhs.zp);
}



inline const Vec3D operator+(const Vec3D& lhs, const Vec3D& rhs) {
  return Vec3D(lhs.xp+rhs.xp, lhs.yp+rhs.yp, lhs.zp+rhs.zp);
}



inline const Vec3D operator-(const Vec3D& lhs, const Vec3D& rhs) {
  return Vec3D(lhs.xp-rhs.xp, lhs.yp-rhs.yp, lhs.zp-rhs.zp);
}



inline const Vec3D operator*(const Vec3D& lhs, const double& rhs) {
  return Vec3D(lhs.xp*rhs, lhs.yp*rhs, lhs.zp*rhs);
}



inline const Vec3D operator/(const Vec3D& lhs, const double& rhs) {
  double v = 1 / rhs;
  return Vec3D(lhs.xp*v, lhs.yp*v, lhs.zp*v);
}



inline bool operator==(const Vec3D& lhs, const Vec3D& rhs) {
  return (lhs.xp==rhs.xp && lhs.yp==rhs.yp && lhs.zp==rhs.zp);
}



inline bool operator!=(const Vec3D& lhs, const Vec3D& rhs) {
  return (lhs.xp!=rhs.xp || lhs.yp!=rhs.yp || lhs.zp!=rhs.zp);
}



inline JDataStream& operator>>(JDataStream& str, Vec3D& rhs) { 
  return str >> rhs.xp >> rhs.yp >> rhs.zp;
}



inline JDataStream& operator<<(JDataStream& str, const Vec3D& rhs) { 
  return str << rhs.xp << rhs.yp << rhs.zp;
}



inline JTextStream& operator<<(JTextStream& str, const Vec3D& rhs) { 
  return str << "[ " << rhs.xp << " " << rhs.yp << " " << rhs.zp << " ]";
}



inline double Vec3D::dotProduct(const Vec3D& lhs, const Vec3D& rhs) { 
  return lhs.xp*rhs.xp + lhs.yp*rhs.yp + lhs.zp*rhs.zp; 
}



inline const Vec3D Vec3D::crossProduct(const Vec3D& lhs, const Vec3D& rhs) {
  return Vec3D(lhs.y()*rhs.z()-rhs.y()*lhs.z(), rhs.x()*lhs.z()-lhs.x()*rhs.z(), lhs.x()*rhs.y()-rhs.x()*lhs.y()); 
}



} } // namespace jude::3d

#endif // _JUDE_3D_VEC3D_H

