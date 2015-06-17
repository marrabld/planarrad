
// vec3d.cc

#include <math.h>
#include <stdio.h>

#include "vec3d.h"
#include "base/jtypes.h"
#include "base/jdatastream.h"
#include "math/rand.h"


using namespace jude::base;
using namespace jude::math;
using namespace jude::S3;



/*!
\brief Returns the length of the vector.
*/

double Vec3D::length() const {
  return sqrt(xp*xp + yp*yp + zp*zp);
}



/*!
\brief Returns the length of the vector squared.

Note it can often be quicker to deal with squared lengths of vectors. For example, when comparing the length of two vectors to determine which is the longest then comparing the sqaure of their lengths is preferred, since it saves the compuation of two square roots.
*/

double Vec3D::lengthSQ() const {
  return xp*xp + yp*yp + zp*zp;
}



/*! \brief Sets the length of the vector to \a len. The direction of the vector from (0,0,0) is preserved.
  If the vector is (0,0,0) it is unchanged. */

void Vec3D::setLength(double len) {
  double f = sqrt(xp*xp + yp*yp + zp*zp);
  if (f != 0) f = len / f;;
  xp *= f;
  yp *= f;
  zp *= f;
}



/*! \brief Sets the length of the vector to 1. The direction of the vector from (0,0,0) is preserved.
  If the vector is (0,0,0) it is unchanged. */

void Vec3D::setUnitLength() {
  double f = sqrt(xp*xp + yp*yp + zp*zp);
  if (f != 0) f = 1.0 / f;
  xp *= f;
  yp *= f;
  zp *= f;
}



// Vec3D::squaredDistance()
/**
Returns the square of the distance of this point from <i>p</i>.
In some cases, for example searching for the closest point <i>p</i>, the square of the distance is sufficient and is more efficient than actual distance as it avoids the sqaure root calculation.
*/

double Vec3D::distanceSQ(const Vec3D& p) const {
  double xv = xp - p.x();
  double yv = yp - p.y();
  double zv = zp - p.z();
  return xv*xv + yv*yv + zv*zv;
}



// Vec3D::distance()
/**
Returns the distance of this point from <i>p</i>.
*/

double Vec3D::distance(const Vec3D& p) const {
  double xv = xp - p.x();
  double yv = yp - p.y();
  double zv = zp - p.z();
  return sqrt(xv*xv + yv*yv + zv*zv);
}





void Vec3D::makeNormal(const Vec3D& v0, const Vec3D& v1) {
  xp = v0.y() * v1.z() - v1.y() * v0.z();
  yp = v1.x() * v0.z() - v0.x() * v1.z();
  zp = v0.x() * v1.y() - v1.x() * v0.y();
  setLength(1);
}



// Vec3D::makeUnit()
// make this vector a unit vector in the direction defined by the 
// zenith angle (theta) and azimuthal angle (phi)

// the relations of x,y,z,theta and phi follow the scheme defined in Mobley 1994, eqn. 1.7
// x (i1) points in the wind direction (where appropriate)
// positive z (i3) is downwards
// y (i2) is in the crosswind direction following the right hand rule

// (theta) is the angle (in radians) between the positive z-direction and the direction vector, 0 <= theta <= PI
// (phi) is the azimuth angle (in radians) between positive x and the direction vector in a 
// clockwise direction when looking along z, 0 <= phi <= 2PI

void Vec3D::makeUnit(double theta, double phi) {
  zp = cos(theta);
  xp = sin(theta) * cos(phi);
  yp = sin(theta) * sin(phi);
}



// Vec3D::thetaFromUnit()
// return the elevation angle (theta) of a unit vector
// only works if the vector has unit length
// test to make sure nan is not returned if zp is very slightky over 1.0
/*! \todo this test was wrong no? */

double Vec3D::thetaFromUnit() const {
  if (zp > 1.0) return 0;
  if (zp < -1.0) return M_PI/2;
  return acos(zp);
  //return (zp>1.0) ? M_PI/2 : acos(zp);
}



// Vec3D::phiFromUnit()
// return the azimuthal angle (phi) of a unit vector
// only works if the vector has unit length

double Vec3D::phiFromUnit() const {
  if (xp==0 && yp==0) return 0;
  if (xp>0) {
    if (yp>=0) return atan(yp/xp); 
    else { 
      double v = (2*M_PI)+atan(yp/xp);
      if (v>=2*M_PI) v -= 2*M_PI;
      return v;
    }
  }
  if (xp<0) return M_PI+atan(yp/xp);
  return (yp>0) ? M_PI/2 : (3*M_PI)/2;
}



// Vec3D::makePointAlongVector()
/**
Makes this vector a point <i>m</i> fraction of the distance along the vector from <i>p0</i> to <i>p1</i>.
*/

void Vec3D::makePointAlongVector(double m, const Vec3D& p0, const Vec3D& p1) { 
  xp = p0.xp + (p1.xp-p0.xp) * m; 
  yp = p0.yp + (p1.yp-p0.yp) * m; 
  zp = p0.zp + (p1.zp-p0.zp) * m;
}



// Vec3D::angle()
/**
Returns the angle in radians bewteen the vector and <i>p0</i>
*/

double Vec3D::angle(const Vec3D& p0) const {
  double ca = (xp*p0.x() + yp*p0.y() + zp*p0.z()) / (length()*p0.length());
  return acos(ca);
}



// Vec3D::rotateXY()

void Vec3D::rotateXY(double angle) {
  double xp2 = xp*cos(angle) - yp*sin(angle);
  yp = xp*sin(angle) + yp*cos(angle);
  xp = xp2;
}

  

// Vec3D::rotateXZ()

void Vec3D::rotateXZ(double angle) {
  double xp2 = xp*cos(angle) - zp*sin(angle);
  zp = xp*sin(angle) + zp*cos(angle);
  xp = xp2;
}

  

// Vec3D::rotateYZ()

void Vec3D::rotateYZ(double angle) {
  double yp2 = yp*cos(angle) - zp*sin(angle);
  zp = yp*sin(angle) + zp*cos(angle);
  yp = yp2;
}



/*! \brief Returns the positions on the line segments \a p1 to \a p2 and \a p3 to \a p4 where is their closest distance. 

  \todo rewrite this code on terms of Vec3D operations
*/

bool Vec3D::minDistanceLineSegments(const Vec3D& p1, const Vec3D& p2, const Vec3D& p3, const Vec3D& p4, double& mu1, double& mu2) {

   double p13_x = p1.x() - p3.x();
   double p13_y = p1.y() - p3.y();
   double p13_z = p1.z() - p3.z();
   double p43_x = p4.x() - p3.x();
   double p43_y = p4.y() - p3.y();
   double p43_z = p4.z() - p3.z();

   if (fabs(p43_x) < 1E-10 && fabs(p43_y) < 1E-10 && fabs(p43_z) < 1E-10) return false;

   double p21_x = p2.x() - p1.x();
   double p21_y = p2.y() - p1.y();
   double p21_z = p2.z() - p1.z();

   if (fabs(p21_x) < 1E-10 && fabs(p21_y) < 1E-10 && fabs(p21_z) < 1E-10) return false;

   double d1343 = p13_x * p43_x + p13_y * p43_y + p13_z * p43_z;
   double d4321 = p43_x * p21_x + p43_y * p21_y + p43_z * p21_z;
   double d1321 = p13_x * p21_x + p13_y * p21_y + p13_z * p21_z;
   double d4343 = p43_x * p43_x + p43_y * p43_y + p43_z * p43_z;
   double d2121 = p21_x * p21_x + p21_y * p21_y + p21_z * p21_z;

   double denom = d2121 * d4343 - d4321 * d4321;
   if (fabs(denom) < 1E-10) return false;

   double top = d1343 * d4321 - d1321 * d4343;

   mu1 = top / denom;
   mu2 = (d1343 + d4321 * mu1) / d4343;

   return true;
}




/*! Returns a Vec3D which has random direction evenly distributed over the hemisphere of all directions with z >= 0. 

  \todo Need to improve the implementation of this and write some test code.
*/

Vec3D Vec3D::hemisphereRand() {
  Vec3D v;
  v.makeUnit(acos(uniform_dbn(0,1)), uniform_dbn(0,2*M_PI));
  return v;
}



/*! Returns a Vec3D which has random direction evenly distributed over the sphere of all directions. 

  \todo Need to improve the implementation of this and write some test code.
*/

Vec3D Vec3D::sphereRand() {
  Vec3D v;
  v.makeUnit(acos(uniform_dbn(-1,1)), uniform_dbn(0,2*M_PI));
  return v;
}



/*! Returns a Vec3D which has random direction evenly distributed over the hemisphere orientated with \a axis pointing to the top. 

  \todo Need to improve the implementation of this and write some test code.
*/

Vec3D Vec3D::hemisphereRand(const Vec3D& axis) {
    // generate a random vector on the sphere
  Vec3D v(sphereRand());
    // if it points the wrong way flip it round
  if (dotProduct(v, axis) < 0) v.negate();
  return v;
}



/*! \brief Returns a vector of unit length at right angles to this vector treated as a unit vector from the origin. 

  Since there are multiple solutions in a plane so the exact direction is unspecified.

  \todo This is only used by rad::PhaseFunc so probably could be made more general and useful.
*/

const Vec3D Vec3D::unitVectorAtRightAngles() const { 
 
  double vx,vy,vz;

  if (xp!=0) {
    double nxny = xp*xp + yp*yp;
    vz = sqrt(nxny);
    vy = - (vz * yp * zp) / nxny;
    vx = - (vy*yp + vz*zp) / xp;

  } else if (yp!=0) {
    double nxny = xp*xp + yp*yp;
    vz = sqrt(nxny);
    vx = - (vz * xp * zp) / nxny;
    vy = - (vx*xp + vz*zp) / yp;

  } else {
    double nxnz = xp*xp + zp*zp;
    vy = sqrt(nxnz);
    vx = - (vy* xp * yp) / nxnz;
    vz = - (vx*xp + vy*yp) / zp;
  }

  return Vec3D(vx,vy,vz);
}



void Vec3D::read(JDataStream& str) {
  str >> xp >> yp >> zp;
}



void Vec3D::write(JDataStream& str) const {
  str << xp << yp << zp;
}



// Vec3D::display()
/*!
\brief Output the x,y and z values of the vector as text.

\todo Needs to be standardised to write to a JTextStream, console by default.
*/

void Vec3D::display() const {
  printf ("x= %f y= %f z= %f\n",xp,yp,zp);
}



/*! \fn Vec3D::Vec3D(double theta, double phi) 
\brief Constructs from a supplied zenith angle \a theta and azimuth angle \a phi. 

Angle values are supplied in radians. 
The vector is generated with z-component being cos(\a theta) and \a phi descriibing the clockwise angle from the x-axis when looking the z-direction.
Note all co-ordinate systems are considered right-handed.
\image html vec3d_from_angles.png
*/

