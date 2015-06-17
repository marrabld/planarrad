
#include "math/ublas.h"
#include "hsparttable.h"
#include "spherepartvxh.h"
#include "dsquadipol.h"
//#include "da_nodeiter.h"
#include "math/mathutil.h"
#include "base/jlog.h"

#include <stdio.h>
#include <assert.h>

using namespace jude::base;
using namespace jude::ds;
using namespace jude::math;



// HSPartTable::(constructor)
// (tp) theta boundaries in radians

HSPartTable::HSPartTable(int vn, int hn, const std::vector<double>& tp_deg) : 
  ft(hn,vn),
  vert_angle(0),
  cap_angle(0),
  is_regular(false),
  adap_array(0) {
  
    // theta points in degrees
  theta_points_deg = tp_deg;
    // convert theta points to radians
  theta_points_rad.resize(theta_points_deg.size());
  for (unsigned int i=0;i<theta_points_deg.size();i++) theta_points_rad[i] = degToRad(theta_points_deg[i]);
 
  buildTable(); 
}



// HSPartTable::buildTable() [private]

void HSPartTable::buildTable() {

  if (jlog::vbLevel(5)) jlog::ls.printf("HSPartTable build %p v %d h %d %s\n", this, ft.ySize(), ft.xSize(), isRegular() ? "Reg" : "Non-Reg");

  quad_array = new SpQuad[units()];

  horiz_angle = (2 * M_PI) / ft.xSize();

  if (isRegular()) {
      // angle steps and cap angle
    vert_angle = M_PI / (2 * ft.ySize() + 1);
    cap_angle = vert_angle / 2;
      // build theta points list
    theta_points_deg.resize(ft.ySize()+2);
    theta_points_deg[0] = 0;
    for (int i=0;i<ft.ySize();i++) theta_points_deg[1+i] = radToDeg(vert_angle * i + cap_angle);
    theta_points_deg[ft.ySize()+1] = 90;
  }

    // convert theta points to radians
  theta_points_rad.resize(theta_points_deg.size());
  for (unsigned int i=0;i<theta_points_deg.size();i++) theta_points_rad[i] = degToRad(theta_points_deg[i]);

    // index zero is an end cap
  quad_array[0].setTo(SpQuad::TOP_CAP, 0, thetaMinRad(0), 0, 2*M_PI);

    // check sums
  double sum_sa = quad_array[0].solidAngle();
  double sum_sact = quad_array[0].solidAngleAbsCosTheta();

    // the other indices are quads
  for (int i=1;i<units();i++) {
    
    int vp,hp;
    ft.indexToPosition(i-1,hp,vp);
 
      // the centre direction in terms of theta and phi
    //double theta = (((double)vp)+0.5) * thetaAngularWidth() + cap_angle;
    //double phi = (((double)hp)+0.5) * phiAngularWidth();
   
      // the theta limits of the quad
    //double theta_min = ((double)vp) * thetaAngularWidth() + cap_angle;
    //double theta_max = theta_min + thetaAngularWidth();
    double theta_min = thetaMinRad(vp);
    double theta_max = thetaMaxRad(vp);

      // the phi limits of the quad
    double phi_min = (hp - 0.5) * phiAngularWidth();
    double phi_max = phi_min + phiAngularWidth();

    quad_array[i].setTo(SpQuad::QUAD, theta_min, theta_max, phi_min, phi_max);

      // check sums
    sum_sa += quad_array[i].solidAngle();
    sum_sact += quad_array[i].solidAngleAbsCosTheta();
  }

    // adaptive node base - starts at index 1
  //ab.setTo(0, 1, ft.xSize(), ft.ySize());

  double sa_correc = 2*M_PI / sum_sa;
  double sact_correc = M_PI / sum_sact;

  if (jlog::vbLevel(5)) {
    jlog::ls.printf("HS %d %d SA sum %f (correc %f)\n", ft.ySize(), ft.xSize(), sum_sa, sa_correc);
    jlog::ls.printf("HS %d %d SACT sum %f (correc %f)\n", ft.ySize(), ft.xSize(), sum_sact, sact_correc);
  }
}



// HSPartTable::direcToIndex()
// (out_direc) should be a unit vector

int HSPartTable::direcToIndex(const Vec3D& out_direc) const {

    // sanity checks
  assert(out_direc.z() >= 0);
  assert(out_direc.x()!=0 || out_direc.y()!=0 || out_direc.z()!=0);

  if (!isRegular()) return direcToIndex_NonRegular(out_direc);

    // get the angle between the vector and the positive z direction
  double theta = out_direc.thetaFromUnit();
    // deal with top cap
  if (theta < cap_angle) return 0;
    // the row in the table
  int v = (int) ((theta-cap_angle) / vert_angle);
    // fix just in case looking exactly on edge - probably unnecessary
    // only introduced due to meshview bug
  if (v == vertSize()) v--;
    // get the angle between the vector and the positive x direction 
  double phi = out_direc.phiFromUnit();
    // the column in the table
  //int h = (int) (phi / phiAngularWidth());
  int h = (int) ((phi / phiAngularWidth()) + 0.5);
  if (h == horizSize()) h = 0;
    // return the index 
  return ft.positionToIndex(h,v) + 1;
}



// HSPartTable::direcToIndex_NonRegular() [private]

int HSPartTable::direcToIndex_NonRegular(const Vec3D& out_direc) const {

    // get the angle between the vector and the positive z direction
  double theta = out_direc.thetaFromUnit();
    // deal with top cap 
  if (theta < theta_points_rad[1]) return 0;
    // the row in the table
  int v = vertSize()-1;
  for (; v>0 && theta_points_rad[v+1]>theta; v--);
    // get the angle between the vector and the positive x direction 
  double phi = out_direc.phiFromUnit();
    // the column in the table
  //int h = (int) (phi / phiAngularWidth());
  int h = (int) ((phi / phiAngularWidth()) + 0.5);
  if (h == horizSize()) h = 0;
    // return the index 
  return ft.positionToIndex(h,v) + 1;
}



// HSPartTable::direcToInterpolated()
/*! \todo FIXME */

void HSPartTable::direcToInterpolated(const Vec3D& out_direc, DsQuadIpol& dqi) const {

    // get the angle between the vector and the positive z direction
  double theta = out_direc.thetaFromUnit();
    // the first relevant row in the table
  double vpos;
  if (isRegular()) vpos = ((theta-cap_angle) / vert_angle) - 0.5;
  else { vpos=0; /* to do */ }
    // get the angle between the vector and the positive x direction 
  double phi = out_direc.phiFromUnit();
    // the first relevant column in the table
  double hpos = (phi / phiAngularWidth()) - 0.5;

    // fractional part of position in this quad
  double vfrac = frac(vpos);
  double hfrac = frac(hpos);

  vfrac = vfrac;
  hfrac = hfrac;

    // set up indices and fractional weights
  /*
  dqi.index0 = indexFromPos(hpos, vpos);
  dqi.frac0 = (1-vfrac) * (1-hfrac);
  dqi.index1 = indexFromPos(hpos+1, vpos);
  dqi.frac1 = (1-vfrac) * hfrac;
  dqi.index2 = indexFromPos(hpos, vpos+1);
  dqi.frac2 = vfrac * (1-hfrac);
  dqi.index3 = indexFromPos(hpos+1, vpos+1);
  dqi.frac3 = vfrac * hfrac;
  */
}



// HSPartTable::indexFromPos() [private]

int HSPartTable::indexFromPos(double hpos, double vpos) const {
    // top cap
  if (vpos < 0) return 0;
    // clamp to bottom edge
  if (vpos > ft.ySize()-1) vpos = ft.ySize()-1;
    // wrap the horizontal position
  if (hpos < 0) hpos += ft.xSize();
  if (hpos >= ft.xSize()) hpos -= ft.xSize();
     // return the index 
  return ft.positionToIndex((int)hpos,(int)vpos) + 1;
} 



// HSPartTable::azimuthallyRotatedIndex()

int HSPartTable::azimuthallyRotatedIndex(int rotated_pos, int index) const {

    // end caps are the same under azimuthal rotation
  if (index==0) return index; 
    // decompose into vertical and horizontal positions
  int hp,vp;
  ft.indexToPosition(index-1, hp, vp);
    // shift horizontal position by rotated_pos
  hp += rotated_pos;
  if (hp >= horizSize()) hp -= horizSize();
    // convert back to index
  return ft.positionToIndex(hp, vp) + 1;
}

