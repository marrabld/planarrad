
// basis3d.cc

#include "S3/basis3d.h"
#include "math/mathutil.h"

using namespace jude::math;
using namespace jude::S3;

//#include "dumpstreamin.h"
//#include "dumpstreamout.h"



// static member

const Basis3D Basis3D::AxesAligned(Vec3D(1,0,0), Vec3D(0,1,0), Vec3D(0,0,1));



// Basis3D::dumpWrite() [static] 
/*
void Basis3D::dumpWrite(Basis3D* bas, DumpStreamOut& str) { 
    // write the object
  bas->write(str); 
    // write the address of the object
  str << (void*) bas;
}
*/


// Basis3D::dumpCreate() [static] 
// only one type of Basis3D
/*
Basis3D* Basis3D::dumpCreate(DumpStreamIn& str) { 
  return str.assocAddress(new Basis3D(str)); 
}
*/



/* Rotates the basis clockwise about the view direction by \a rad radians. */

void Basis3D::rotateAboutZ(double rad) {
  Vec3D nx(cos(-rad), -sin(-rad), 0);
  Vec3D ny(sin(-rad), cos(-rad), 0);
  nx = outOfBasis(nx);
  ny = outOfBasis(ny);
  x_axis = nx;
  y_axis = ny;
}



/* Rotates the basis clockwise about the external space y-axis by \a rad radians. */

void Basis3D::rotateAboutExternalX(double rad) {
  Vec3D::rotate(rad, y_axis, z_axis);
}



/* Rotates the basis clockwise about the external space y-axis by \a rad radians. */

void Basis3D::rotateAboutExternalY(double rad) {
  Vec3D::rotate(rad, x_axis, z_axis);
}



/* Rotates the basis clockwise about the external space z-axis by \a rad radians. */

void Basis3D::rotateAboutExternalZ(double rad) {
  Vec3D::rotate(rad, x_axis, y_axis);
}



/* Sets the Basis z-axis to \a vz and the x and y axes in the perpendicular plane with rotation set according to the other parameters.

*/

void Basis3D::setFromZDirec(const Vec3D& vz, 
			    bool xaxis_in_xy_plane, bool xaxis_in_yz_plane, bool xaxis_in_xz_plane, 
			    bool yaxis_on_positive_side, double rot_deg) {


    // set z-axis
  z_axis = vz;
    // a additional rotation of the x-y axes clockwise looking in the view direction, which is applied last 
  double rot_rad = degToRad(rot_deg);
    // for convienince this is the view direction
  const Vec3D& direc(z_axis);
    // one of the three constraints must be true
  if (!xaxis_in_yz_plane && !xaxis_in_xz_plane) xaxis_in_xy_plane = true;


    // the basis x-axis is constrained to lie in the external space co-ord system x-y plane
  if (xaxis_in_xy_plane) {

    double dn = sqrt(direc.x()*direc.x() + direc.y()*direc.y());
    x_axis = Vec3D(direc.y()/dn, -direc.x()/dn, 0);
    //if (view_x.x() < 0) view_x.negate();
    y_axis = Vec3D::crossProduct(direc, x_axis);
    y_axis.setUnitLength();
    if ((y_axis.z() > 0) != yaxis_on_positive_side) rot_rad += M_PI;


    // the basis x-axis is constrained to lie in the external space co-ord system y-z plane
  } else if (xaxis_in_yz_plane) {

    double dn = sqrt(direc.y()*direc.y() + direc.z()*direc.z());
    x_axis = Vec3D(0, direc.z()/dn, -direc.y()/dn);
    //if (view_x.y() < 0) view_x.negate();
    y_axis = Vec3D::crossProduct(direc, x_axis);
    y_axis.setUnitLength();
    if ((y_axis.x() > 0) != yaxis_on_positive_side) rot_rad += M_PI;
    

    // the basis x-axis is constrained to lie in the external space co-ord system x-z plane
  } else if (xaxis_in_xz_plane) {
  
    double dn = sqrt(direc.x()*direc.x() + direc.z()*direc.z());
    x_axis = Vec3D(direc.z()/dn, 0, -direc.x()/dn);
    //if (view_x.x() < 0) view_x.negate();
    y_axis = Vec3D::crossProduct(direc, x_axis);
    y_axis.setUnitLength();
    if ((y_axis.y() > 0) != yaxis_on_positive_side) rot_rad += M_PI;
  }

  rotateAboutZ(rot_rad);  
}



// Basis3D::write()

void Basis3D::write(JDataStream& str) const { 
  x_axis.write(str);
  y_axis.write(str);
  z_axis.write(str);
}



// Basis3D::read()

void Basis3D::read(JDataStream& str) {

  x_axis.read(str);
  y_axis.read(str);
  z_axis.read(str);
}



// Basis3D::copy()

void Basis3D::copy(const Basis3D& b) {
  x_axis = b.x_axis;
  y_axis = b.y_axis;
  z_axis = b.z_axis;
}



// Basis3D::makeFromNotUnits()

void Basis3D::makeFromNotUnits(const Vec3D& x, const Vec3D& y) {
  x_axis = x;
  x_axis.setLength(1);
  y_axis = y;
  y_axis.setLength(1);
  z_axis = Vec3D::crossProduct(x_axis, y_axis);
}



// Basis3D::makeFromNotUnits()

void Basis3D::makeFromNotUnits(const Vec3D& x, const Vec3D& y, const Vec3D& z) {
  x_axis = x;
  x_axis.setLength(1);
  y_axis = y;
  y_axis.setLength(1);
  z_axis = z;
  z_axis.setLength(1);
}



// Basis3D::intoBasis()
// project an extranal directonal vector into the basis co-ordinate system

Vec3D Basis3D::intoBasis(const Vec3D& v) const {
  double x = Vec3D::dotProduct(v, x_axis);
  double y = Vec3D::dotProduct(v, y_axis);
  double z = Vec3D::dotProduct(v, z_axis);
  return Vec3D(x,y,z);
}



// Basis3D::outOfBasis()
/*
Vector3D Basis3D::outOfBasis(const Vector3D& v) const {
  double x = v.x()*x_axis.x() + v.y()*y_axis.x() + v.z()*z_axis.x();
  double y = v.x()*x_axis.y() + v.y()*y_axis.y() + v.z()*z_axis.y();
  double z = v.x()*x_axis.z() + v.y()*y_axis.z() + v.z()*z_axis.z();
  return Vector3D(x,y,z);
}
*/


Vec3D Basis3D::outOfBasis(const Vec3D& v) const {
  Vec3D tx(x_axis.x(), y_axis.x(), z_axis.x());
  Vec3D ty(x_axis.y(), y_axis.y(), z_axis.y());
  Vec3D tz(x_axis.z(), y_axis.z(), z_axis.z());
  tx.setLength(1);
  ty.setLength(1);
  tz.setLength(1);
  double ix = Vec3D::dotProduct(v, tx);
  double iy = Vec3D::dotProduct(v, ty);
  double iz = Vec3D::dotProduct(v, tz);
  //return Vector3D(ix,iy,iz).setUnitLength();
  return Vec3D(ix,iy,iz);
}



// Basis3D::checkOk()

bool Basis3D::checkOk() const {

  const double err = 1E-6;

  double xl = x_axis.length();
  double yl = y_axis.length();
  double zl = z_axis.length();

  if (xl<1-err || xl>1+err || yl<1-err || yl>1+err || zl<1-err || zl>1+err) return false;

  return true;
}



// Basis3D::display()

void Basis3D::display(JTextStream& str) const {
  str << "x-axis: " << x_axis << "\n";
  str << "y-axis: " << y_axis << "\n";
  str << "z-axis: " << z_axis << "\n";
}

