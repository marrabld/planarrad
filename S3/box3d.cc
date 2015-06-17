
#include "box3d.h"

using namespace jude::S3;



// Box3D::intersectionDistances()

bool Box3D::intersectionDistances(const Vec3D& start, const Vec3D& dv, double& d_near, double& d_far) const {

  double x_min = xMin();
  double x_max = xMax();
  double y_min = yMin();
  double y_max = yMax();
  double z_min = zMin();
  double z_max = zMax();

  d_near = -HUGE_VALF;
  d_far = HUGE_VALF;
 
  double dt_in, dt_out;

  if (!dv.x()) { 
    if (start.x()<x_min || start.x()>x_max) return false;
  } else {
    if (dv.x() > 0) {
      dt_in = (x_min - start.x()) / dv.x();
      dt_out = (x_max - start.x()) / dv.x();
    } else {
      dt_in = (x_max - start.x()) / dv.x();
      dt_out = (x_min - start.x()) / dv.x();
    }
    if (dt_in > d_near) d_near = dt_in;
    if (dt_out < d_far) d_far = dt_out;
  }
  
  if (!dv.y()) {
    if (start.y()<y_min || start.y()>y_max) return false;
  } else {
    if (dv.y() > 0) {
      dt_in = (y_min - start.y()) / dv.y();
      dt_out = (y_max - start.y()) / dv.y();
    } else {
      dt_in = (y_max - start.y()) / dv.y();
      dt_out = (y_min - start.y()) / dv.y();
    }
    if (dt_in > d_near) d_near = dt_in;
    if (dt_out < d_far) d_far = dt_out;
  }

  if (!dv.z()) {
    if (start.z()<z_min || start.z()>z_max) return false;
  } else {
    if (dv.z() > 0) {
      dt_in = (z_min - start.z()) / dv.z();
      dt_out = (z_max - start.z()) / dv.z();
    } else {
      dt_in = (z_max - start.z()) / dv.z();
      dt_out = (z_min - start.z()) / dv.z();
    }
    if (dt_in > d_near) d_near = dt_in;
    if (dt_out < d_far) d_far = dt_out;
  }

  return (d_near < d_far);
}

