
#include "spquad.h"
#include "S3/box3d.h"
#include "base/jtypes.h"
#include <assert.h>
//#include "error.h"

using namespace jude::ds;
using namespace jude::S3;



// SpQuad::(constructor)

SpQuad::SpQuad(const SpQuad* q0, const SpQuad* q1, const SpQuad* q2, const SpQuad* q3) : 
  theta_min(HUGE_VALF),
  theta_max(-HUGE_VALF),
  phi_min(HUGE_VALF),
  phi_max(-HUGE_VALF) {

    // all quads should not be null
  assert(q0 || q1 || q2 || q3);

  if (q0) {
    theta_min = min(theta_min, q0->thetaMin());
    theta_max = max(theta_max, q0->thetaMax());
    phi_min = min(phi_min, q0->phiMin());
    phi_max = max(phi_max, q0->phiMax());
  }

  if (q1) {
    theta_min = min(theta_min, q1->thetaMin());
    theta_max = max(theta_max, q1->thetaMax());
    phi_min = min(phi_min, q1->phiMin());
    phi_max = max(phi_max, q1->phiMax());
  }

  if (q2) {
    theta_min = min(theta_min, q2->thetaMin());
    theta_max = max(theta_max, q2->thetaMax());
    phi_min = min(phi_min, q2->phiMin());
    phi_max = max(phi_max, q2->phiMax());
  }

  if (q3) {
    theta_min = min(theta_min, q3->thetaMin());
    theta_max = max(theta_max, q3->thetaMax());
    phi_min = min(phi_min, q3->phiMin());
    phi_max = max(phi_max, q3->phiMax());
  }

  setTo(QUAD, theta_min, theta_max, phi_min, phi_max);
}



// SpQuad::setTo()

void SpQuad::setTo(int type, double tmin, double tmax, double pmin, double pmax) {

  theta_min = tmin;
  theta_max = tmax;
  phi_min = pmin;
  phi_max = pmax;

  corn0 = Vec3D(theta_min, phi_min);
  corn1 = Vec3D(theta_min, phi_max);
  corn2 = Vec3D(theta_max, phi_max);
  corn3 = Vec3D(theta_max, phi_min);
  
    // note this also works for end caps where it reduces to 1-cos(cap_angle)
  solid_angle = ((cos(theta_min) - cos(theta_max)) * (phi_max - phi_min)); 
  
  mean_cos_theta = (cos(theta_min) + cos(theta_max)) / 2;

  //printf("theta_min %f theta_max %f sa %f\n",tmin,tmax,solid_angle);

  switch (type) {

  case TOP_CAP : 
    mean_theta = 0;
    point_theta = 0;
    phi_centre = 0;
    is_end_cap = true;
    point_direc = Vec3D(0,0,1);
    mean_direc = Vec3D(0,0,1);
    mean_cube_distance = 1 / fabs(mean_cos_theta); // assumes end cap half angle < 45 deg
    point_cube_distance = mean_cube_distance;
    break;

  case BOTTOM_CAP : 
    mean_theta = M_PI;
    point_theta = M_PI;
    phi_centre = 0;
    is_end_cap = true;
    point_direc = Vec3D(0,0,-1);
    mean_direc = Vec3D(0,0,-1);
    mean_cube_distance = 1 / fabs(mean_cos_theta); // assumes end cap half angle < 45 deg
    point_cube_distance = mean_cube_distance;
    break;
    
  default: 

    //double sa_calc;
    //int face = 0;
    //centre_direc = Quad3R(corn0,corn1,corn2,corn3).integralOfCosTheta(face,sa_calc);

      // calculate centre direction of quad as centre of mass of the quad (probably best way to do it)
    //if (Params::quadCentreIsCentreOfMass()) centre_direc = Quad3R(corn0,corn1,corn2,corn3).centreOfMass();    
      // or calculate as mean of corner points (old way of doing it for both sphere and cube directional structures)
    //else centre_direc = (corn0 + corn1 + corn2 + corn3) / 4;
    //centre_direc = (corn0 + corn1 + corn2 + corn3) / 4;
 
      // make sure vector is unit length
    //centre_direc.setUnitLength();
      // angular position
    //theta_centre = centre_direc.thetaFromUnit();
    //phi_centre = centre_direc.phiFromUnit();

      // this makes the 'point' direction exactly the same as the mean direction
      // for sphere partitions it doesn't matter since there is no analagous projector partition
      // for cube partitions 'point' direction is the same as fullcube view directions 
      // and not the same as the mean direction
    mean_theta = acos((cos(theta_min)+cos(theta_max)) / 2);
    point_theta = (theta_min + theta_max) / 2;
    phi_centre = (phi_min + phi_max) / 2;
    //point_direc = Vec3D(theta_centre, phi_centre);
    point_direc = Vec3D(point_theta, phi_centre);

      // Note: the below seems a bit long winded way to get the mean direction but maybe it ensures numerical accuracy
      // now calculate mean distance through unit cube based on mean cos theta
      // set z as the cos theta value for this qaud direction
    mean_direc.setZ(mean_cos_theta);
      // calculate x and y of the direction vector based on their ratios
      // in the centre direction. ie. the result vector projects 
      // into the same place in the z=0 plane
    double f = point_direc.y() / point_direc.x();
    mean_direc.setX(sqrt( (1 - mean_direc.z()*mean_direc.z()) / (1 + f*f) ));
    if (point_direc.x() <0) mean_direc.setX(-mean_direc.x());
    mean_direc.setY(f * mean_direc.x());
      // intersect on a double size unit cube starting in the centre 
    Box3D box(Vec3D(-1,-1,-1), Vec3D(1,1,1));
    double dummy;
    box.intersectionDistances(Vec3D(0,0,0), mean_direc, dummy, mean_cube_distance);
    box.intersectionDistances(Vec3D(0,0,0), point_direc, dummy, point_cube_distance);
  }
}



// SpQuad::display()

bool SpQuad::display(JTextStream& str, int index, int code) const {

    // if the base class can display then done
  if (DsQuad::display(str, index, code)) return true;

  //switch (code) {
  //  case DsQuad::SA : str << solidAngle(); return true;
  //}

  return false;
}

