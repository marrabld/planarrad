
// box3d.h

#ifndef _JUDE_D3_BOX3D_H_JDH
#define _JUDE_D3_BOX3D_H_JDH

#include "S3/vec3d.h"

namespace jude {
namespace S3 {



class Box3D {

  Vec3D min_point;   // inclusive
  Vec3D max_point;   // not inclusive

public:

  Box3D() { }

  Box3D(const Vec3D& sz) :
    min_point(0, 0, 0),
    max_point(sz) { }

  Box3D(const Vec3D& minp, const Vec3D& maxp) :
    min_point(minp),
    max_point(maxp) { }

 Box3D(const Box3D& b) :
    min_point(b.min_point),
    max_point(b.max_point) { }

  bool isUndefined() const {
    return (min_point.x()>max_point.x() || min_point.y()>max_point.y() || min_point.z()>max_point.z());
  }

  double diagonal() const { return (min_point - max_point).length(); }

  void initBoundingBox() {
    min_point = Vec3D(DOUBLE_MAX, DOUBLE_MAX, DOUBLE_MAX);
    max_point = Vec3D(DOUBLE_MIN, DOUBLE_MIN, DOUBLE_MIN);
  }

  void expandToInclude(const Vec3D& v) {
    if (v.x() > max_point.x()) max_point.setX(v.x());
    if (v.y() > max_point.y()) max_point.setY(v.y());
    if (v.z() > max_point.z()) max_point.setZ(v.z());
    if (v.x() < min_point.x()) min_point.setX(v.x());
    if (v.y() < min_point.y()) min_point.setY(v.y());
    if (v.z() < min_point.z()) min_point.setZ(v.z());
  }

  void expandToInclude(const Box3D& b) {
    if (b.isUndefined()) return;
    expandToInclude(b.maxPoint());
    expandToInclude(b.minPoint());
  }

  void shift(const Vec3D& s) {
    min_point += s;
    max_point += s;
  }

  double wrapX(double x) const { return x - (floor((x-xMin()) / xSize()) * xSize()); }
  double wrapY(double y) const { return y - (floor((y-yMin()) / ySize()) * ySize()); }
  double wrapZ(double z) const { return z - (floor((z-zMin()) / zSize()) * zSize()); }

  Vec3D wrappedPosition(const Vec3D& v) const { return Vec3D(wrapX(v.x()), wrapY(v.y()), wrapZ(v.z())); }

  double xMin() const { return min_point.x(); }
  double xMax() const { return max_point.x(); }
  double yMin() const { return min_point.y(); }
  double yMax() const { return max_point.y(); }
  double zMin() const { return min_point.z(); }
  double zMax() const { return max_point.z(); }
 
  void setXMin(double x) { min_point.setX(x); }
  void setXMax(double x) { max_point.setX(x); }
  void setYMin(double y) { min_point.setY(y); }
  void setYMax(double y) { max_point.setY(y); }
  void setZMin(double z) { min_point.setZ(z); }
  void setZMax(double z) { max_point.setZ(z); }

  double xSize() const { return max_point.x() - min_point.x(); }
  double ySize() const { return max_point.y() - min_point.y(); }
  double zSize() const { return max_point.z() - min_point.z(); }

  double volume() const { return isUndefined() ? 0 : (xSize() * ySize() * zSize()); }

  const Vec3D& minPoint() const { return min_point; }
  const Vec3D& maxPoint() const { return max_point; }

    // probably depreceated
  const Vec3D v0() const { return min_point; }
  const Vec3D v1() const { return Vec3D(min_point.x(), max_point.y(), min_point.z()); }
  const Vec3D v2() const { return Vec3D(max_point.x(), max_point.y(), min_point.z()); }
  const Vec3D v3() const { return Vec3D(max_point.x(), min_point.y(), min_point.z()); }
  const Vec3D v4() const { return Vec3D(min_point.x(), min_point.y(), max_point.z()); }
  const Vec3D v5() const { return Vec3D(min_point.x(), max_point.y(), max_point.z()); }
  const Vec3D v6() const { return max_point; }
  const Vec3D v7() const { return Vec3D(max_point.x(), min_point.y(), max_point.z()); }

    // use these instead
  const Vec3D v000() const { return min_point; }
  const Vec3D v001() const { return Vec3D(min_point.x(), min_point.y(), max_point.z()); }
  const Vec3D v010() const { return Vec3D(min_point.x(), max_point.y(), min_point.z()); }
  const Vec3D v011() const { return Vec3D(min_point.x(), max_point.y(), max_point.z()); }
  const Vec3D v100() const { return Vec3D(max_point.x(), min_point.y(), min_point.z()); }
  const Vec3D v101() const { return Vec3D(max_point.x(), min_point.y(), max_point.z()); }
  const Vec3D v110() const { return Vec3D(max_point.x(), max_point.y(), min_point.z()); }
  const Vec3D v111() const { return max_point; }

  bool contains(const Vec3D& v) const { 
    return min_point.allLessThanOrEqual(v) && max_point.allGreaterThan(v); 
  }

  bool containsXY(const Vec3D& v) const { 
    return (min_point.x()<=v.x() && min_point.y()<=v.y() && max_point.x()>v.x() && max_point.y()>v.y());
  }

  bool intersects(const Box3D& b) const { 
    return !((xMin() >= b.xMax() || xMax() <= b.xMin()) || \
	     (yMin() >= b.yMax() || yMax() <= b.yMin()) || \
	     (zMin() >= b.zMax() || zMax() <= b.zMin()));
  }

  bool intersectsXY(const Box3D& b) const { 
    return !((xMin() >= b.xMax() || xMax() <= b.xMin()) || \
	     (yMin() >= b.yMax() || yMax() <= b.yMin()));
  }

  bool containsInclusive(const Vec3D& v) const { 
    return min_point.allLessThanOrEqual(v) && max_point.allGreaterThanOrEqual(v); 
  }

  bool intersectionDistances(const Vec3D& start, const Vec3D& dv, double& d_near, double& d_far) const;

    /*! \brief Returns the z-distance * dv.z() to intersection with far z-limit plane. */
  double zOutwardIsecDistance(const Vec3D& v, const Vec3D& dv) const { 
    double z_dist = (dv.z() > 0) ? (zMax() - v.z()) : (zMin() - v.z()); 
    return z_dist / dv.z();
  }

  static const Box3D intersect(const Box3D& b1, const Box3D& b2) {
    Vec3D minp(max(b1.xMin(), b2.xMin()), max(b1.yMin(), b2.yMin()), max(b1.zMin(), b2.zMin()));
    Vec3D maxp(min(b1.xMax(), b2.xMax()), min(b1.yMax(), b2.yMax()), min(b1.zMax(), b2.zMax()));
    return Box3D(minp, maxp);
  }

  friend JTextStream& operator<<(JTextStream& str, const Box3D& rhs);
};



inline JTextStream& operator<<(JTextStream& str, const Box3D& rhs) { 
  return str << rhs.min_point << " " << rhs.max_point;
}



} } // namespace jude::S3

#endif // _JUDE_D3_BOX3D_H

