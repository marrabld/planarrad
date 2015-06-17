
// vec2d.h

#ifndef _JUDE_S2_VEC2D_H
#define _JUDE_S2_VEC2D_H

#include <math.h>
#include "base/jdatastream.h"
#include "base/jtextstream.h"
#include "base/jtypes.h"

namespace jude {
namespace S2 {

using namespace jude::base;



class Vec2D {

protected:

  double xp;
  double yp;

public:

    /*! \brief Construct without initialisation. Contents are undefined. */
  Vec2D() : xp(0), yp(0) { }

    /*! \brief Construct with supplied x,y values. */
  Vec2D(double ix, double iy) : xp(ix), yp(iy) { }

    /*! \brief Construct by reading x,y values from a JDataStream. */
  Vec2D(JDataStream& str) {
    str >> xp;
    str >> yp;
  }

  Vec2D& operator=(const Vec2D& rhs) { 
    xp = rhs.xp;
    yp = rhs.yp;
    return *this;
  }
  
  Vec2D& operator+=(const Vec2D& rhs) { 
    xp += rhs.xp;
    yp += rhs.yp;
    return *this;
  }

  Vec2D& operator-=(const Vec2D& rhs) { 
    xp -= rhs.xp;
    yp -= rhs.yp;
    return *this;
  }
  
  Vec2D& operator*=(double m) { 
    xp *= m;
    yp *= m;
    return *this;
  }

  double x() const { return xp; }
  double y() const { return yp; }
  void setX(double ix) { xp = ix; }
  void setY(double iy) { yp = iy; }

  void setTo(double ix, double iy) { xp=ix; yp=iy; }

  void negate() { 
    xp = -xp; 
    yp = -yp; 
  }

  void add(const Vec2D& p) { 
    xp += p.xp; 
    yp += p.yp; 
  }

  void multiply(double v) { 
    xp *= v; 
    yp *= v; 
  }

  void divide(double v) { 
    xp /= v; 
    yp /= v; 
  }

  void subtract(const Vec2D& p) { 
    xp -= p.xp; 
    yp -= p.yp; 
  }

  void addMultiple(const Vec2D& p, double m) { 
    xp += m*p.xp; 
    yp += m*p.yp; 
  }

  void makeSum(const Vec2D& p0, const Vec2D& p1) { 
    xp = p0.xp + p1.xp; 
    yp = p0.yp + p1.yp;
  }

  void makeSubtraction(const Vec2D& p0, const Vec2D& p1) { 
    xp = p0.xp - p1.xp; 
    yp = p0.yp - p1.yp; 
  }

  void makeSumOfMultiples(const Vec2D& p0, double m0, const Vec2D& p1, double m1) { 
    xp = m0*p0.xp + m1*p1.xp; 
    yp = m0*p0.yp + m1*p1.yp;  
  }

  void makeMidPoint(const Vec2D& p0, const Vec2D& p1) { 
    xp = (p0.xp+p1.xp) * 0.5; 
    yp = (p0.yp+p1.yp) * 0.5; 
  }

  static const Vec2D midPoint(const Vec2D& p0, const Vec2D& p1) { 
    return Vec2D((p0.xp+p1.xp) * 0.5, (p0.yp+p1.yp) * 0.5);
  }
 
  static const Vec2D minPoint(const Vec2D& lhs, const Vec2D& rhs) { 
    return Vec2D(min(lhs.x(), rhs.x()), min(lhs.y(), rhs.y())); 
  }

  static const Vec2D maxPoint(const Vec2D& lhs, const Vec2D& rhs) {
    return Vec2D(max(lhs.x(), rhs.x()), max(lhs.y(), rhs.y())); 
  }
 
  double length() const;
  double lengthSQ() const;
  void setLength(double len);
  void setUnitLength();
  double distanceSQ(const Vec2D& p) const;
  double distance(const Vec2D& p) const;

  friend inline const Vec2D operator-(const Vec2D& rhs);
 
  friend inline const Vec2D operator+(const Vec2D& lhs, const Vec2D& rhs);
  friend inline const Vec2D operator-(const Vec2D& lhs, const Vec2D& rhs);

  friend inline const Vec2D operator*(const Vec2D& lhs, const double& rhs);
  friend inline const Vec2D operator/(const Vec2D& lhs, const double& rhs);

  friend inline bool operator==(const Vec2D& lhs, const Vec2D& rhs);
  friend inline bool operator!=(const Vec2D& lhs, const Vec2D& rhs);
 
  friend JDataStream& operator>>(JDataStream& str, Vec2D& rhs);
  friend JDataStream& operator<<(JDataStream& str, const Vec2D& rhs);
 
  friend JTextStream& operator<<(JTextStream& str, const Vec2D& rhs);
 
  void read(JDataStream& str);
  void write(JDataStream& str) const;

  void display(JTextStream& str) const;
};



inline const Vec2D operator-(const Vec2D& rhs) { 
  return Vec2D(-rhs.xp, -rhs.yp);
}



inline const Vec2D operator+(const Vec2D& lhs, const Vec2D& rhs) {
  return Vec2D(lhs.xp+rhs.xp, lhs.yp+rhs.yp);
}



inline const Vec2D operator-(const Vec2D& lhs, const Vec2D& rhs) {
  return Vec2D(lhs.xp-rhs.xp, lhs.yp-rhs.yp);
}



inline const Vec2D operator*(const Vec2D& lhs, const double& rhs) {
  return Vec2D(lhs.xp*rhs, lhs.yp*rhs);
}



inline const Vec2D operator/(const Vec2D& lhs, const double& rhs) {
  double v = 1 / rhs;
  return Vec2D(lhs.xp*v, lhs.yp*v);
}



inline bool operator==(const Vec2D& lhs, const Vec2D& rhs) {
  return (lhs.xp==rhs.xp && lhs.yp==rhs.yp);
}



inline bool operator!=(const Vec2D& lhs, const Vec2D& rhs) {
  return (lhs.xp!=rhs.xp || lhs.yp!=rhs.yp);
}



inline JDataStream& operator>>(JDataStream& str, Vec2D& rhs) { 
  return str >> rhs.xp >> rhs.yp;
}



inline JDataStream& operator<<(JDataStream& str, const Vec2D& rhs) { 
  return str << rhs.xp << rhs.yp;
}



inline JTextStream& operator<<(JTextStream& str, const Vec2D& rhs) { 
  return str << "[ " << rhs.xp << " " << rhs.yp << " ]";
}



} } // namespace jude::S2

#endif // _JUDE_S2_VEC2D_H

