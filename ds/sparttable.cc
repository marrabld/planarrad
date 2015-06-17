
#include "ds/sparttable.h"
//#include "spherepartvxh.h"
#include "ds/dsquadipol.h"
#include "math/mathutil.h"
#include "base/jlog.h"

#include <stdio.h>

using namespace jude::ds;
using namespace jude::math;



// SPartTable::(constructor)

SPartTable::SPartTable(int vn, int hn, const std::vector<double>& tp_deg) : 
  ft(hn,vn),
  vert_angle(0),
  cap_angle(0),
  is_regular(false),
  adap_array(0),
  sym_index_data(0), 
  sym_trans_set(0) { 

    // theta points in degrees
  theta_points_deg = tp_deg;
    // convert theta points to radians
  theta_points_rad.resize(theta_points_deg.size());
  for (unsigned int i=0;i<theta_points_deg.size();i++) theta_points_rad[i] = degToRad(theta_points_deg[i]);
 
  buildTable(); 
}



// SPartTable::buildTable() [private]

void SPartTable::buildTable() {

  if (jlog::vbLevel(5)) jlog::ls.printf("SPartTable build %p v %d h %d %s\n", this, ft.ySize(), ft.xSize(), isRegular() ? "Reg" : "Non-Reg");

  quad_array = new SpQuad[units()];
  negated_index_list = new int[units()];

  horiz_angle = (2 * M_PI) / ft.xSize();

  if (isRegular()) {
      // angle steps and cap angle
    vert_angle = M_PI / (ft.ySize() + 1);
    cap_angle = vert_angle / 2;
      // build theta points list
    theta_points_deg.resize(ft.ySize()+3);
    theta_points_deg[0] = 0;
    for (int i=0;i<=ft.ySize();i++) theta_points_deg[1+i] = radToDeg(vert_angle * i + cap_angle);
    theta_points_deg[ft.ySize()+2] = 180;
  }

    // convert theta points to radians
  theta_points_rad.resize(theta_points_deg.size());
  for (unsigned int i=0;i<theta_points_deg.size();i++) theta_points_rad[i] = degToRad(theta_points_deg[i]);

    // index zero is an end cap
  quad_array[0].setTo(SpQuad::TOP_CAP, 0, thetaMinRad(0), 0, 2*M_PI);
    // final index is an end cap
  quad_array[units()-1].setTo(SpQuad::BOTTOM_CAP, thetaMaxRad(vertSize()-1), M_PI, 0, 2*M_PI);

    // check sums
  double sum_sa = quad_array[0].solidAngle() + quad_array[units()-1].solidAngle();
  double sum_sact = quad_array[0].solidAngleAbsCosTheta() + quad_array[units()-1].solidAngleAbsCosTheta();
    // the other indices are quads
  for (int i=1;i<units()-1;i++) {
    
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

  double sa_correc = 4*M_PI / sum_sa;
  double sact_correc = 2*M_PI / sum_sact;

  if (jlog::vbLevel(5)) {
    jlog::ls.printf("S %d %d Uncorrected SA sum %f (correc %f)\n", ft.ySize(), ft.xSize(), sum_sa, sa_correc);
    jlog::ls.printf("S %d %d Uncorrected SACT sum %f (correc %f)\n", ft.ySize(), ft.xSize(), sum_sact, sact_correc);
  }

    // build negated direction index table
  for (int i=0;i<units();i++) {
    Vec3D dv(indexToPointDirec(i));
    dv.negate();
    negated_index_list[i] = direcToIndex(dv);
  }
}



// SPartTable::(destructor)

SPartTable::~SPartTable() {
  delete[] quad_array;
  delete[] adap_array;
  delete[] sym_index_data;
  delete[] sym_trans_set;
  delete[] negated_index_list;
}



// SPartTable::direcToIndex()
// (out_direc) should be a unit vector

int SPartTable::direcToIndex(const Vec3D& out_direc) const {

  if (!isRegular()) return direcToIndex_NonRegular(out_direc);

    // get the angle between the vector and the positive z direction
  double theta = out_direc.thetaFromUnit();
    // deal with top cap
  if (theta < cap_angle) return 0;
    // deal with bottom cap
  if (theta >= M_PI-cap_angle) return units()-1;
    // the row in the table
  int v = (int) ((theta-cap_angle) / vert_angle);
    // get the angle between the vector and the positive x direction 
  double phi = out_direc.phiFromUnit();
    // the column in the table
  //int h = (int) (phi / phiAngularWidth());
  int h = (int) ((phi / phiAngularWidth()) + 0.5);
  if (h == horizSize()) h = 0;
    // return the index 
  return ft.positionToIndex(h,v) + 1;
}



// SPartTable::direcToIndex()
// (out_direc) should be a unit vector

int SPartTable::direcToIndex_NonRegular(const Vec3D& out_direc) const {

    // get the angle between the vector and the positive z direction
  double theta = out_direc.thetaFromUnit();
    // deal with top cap
  if (theta < theta_points_rad[1]) return 0;
    // deal with bottom cap
  if (theta >= theta_points_rad[vertSize()+1]) return units()-1;
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



// SPartTable::direcToInterpolated()
/*! \todo FIXME */

void SPartTable::direcToInterpolated(const Vec3D& out_direc, DsQuadIpol& dqi) const {

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



// SPartTable::indexFromPos() [private]

int SPartTable::indexFromPos(double hpos, double vpos) const {
    // top cap
  if (vpos < 0) return 0;
    // bottom cap
  if (vpos >= ft.ySize()) return units()-1;
    // wrap the horizontal position
  if (hpos < 0) hpos += ft.xSize();
  if (hpos >= ft.xSize()) hpos -= ft.xSize();
     // return the index 
  return ft.positionToIndex((int)hpos,(int)vpos) + 1;
} 



// SPartTable::azimuthallyRotatedIndex()

int SPartTable::azimuthallyRotatedIndex(int rotated_pos, int index) const {

    // end caps are the same under azimuthal rotation
  if (index==0 || index==units()-1) return index; 
    // decompose into vertical and horizontal positions
  int hp,vp;
  ft.indexToPosition(index-1, hp, vp);
    // shift horizontal position by rotated_pos
  hp += rotated_pos;
  if (hp >= horizSize()) hp -= horizSize();
    // convert back to index
  return ft.positionToIndex(hp, vp) + 1;
}



// SPartTable::buildSymmetryTable() [private]
// the sphere partition can be re-orientated in 4*horizum() ways (numbered 0-4*horizNum())
// for each orientation each index number is translated to another index number

void SPartTable::buildSymmetryTable() const {

    // this table holds the translated index numbers for 
    // each index for each of the orientations
  sym_index_data = new int[4 * horizSize() * units()];

    // for a given in-index there will be one or two translations that will turn the in-index
    // into a 'standard position' in-index (lowest possible index number of a homogenous quad)
    // this table holds the numbers of the two translations (0-47) for each in-index
    // if there is only one such translation both entries will be the same
  sym_trans_set = new int[2 * units()];

}



// SPartTable::symmetricalIndices()

const int* SPartTable::symmetricalIndices(int index) const {

  if (!sym_index_data) buildSymmetryTable();

  return sym_index_data + (index * 4 * horizSize());

  return 0;
}



// SPartTable::symmetricalIndicesConvert()
// converts (in_index) and (out_index) to 'standard position'

void SPartTable::symmetricalIndicesConvert(int& in_index, int& out_index) const {

  if (!sym_index_data) buildSymmetryTable();

}



// SPartTable::symmetricalNodesRepresented()

int SPartTable::symmetricalNodesRepresented(int in_index, int out_index) const {

  if (!sym_index_data) buildSymmetryTable();

  return 1;
}



// SPartTable::display()
/*
void SPartTable::display(JTextStream& str, int code) const {

  str << "SpherePartitionVxH " << ft.ySize() << " x " << ft.xSize() << (isRegular() ? "Reg" : "Non-Reg") << "\n";

  str << "Vertical Theta Boundaries:\n";

  Params::setFormat(FORMAT_DS_THETA_DEG, str);
  str << "(degrees) ";
  for (int vp=0;vp<=ft.ySize()+1;vp++) str << theta_points_deg[vp] << ", ";
  str << theta_points_deg[ft.ySize()+2] << "\n";

  Params::setFormat(FORMAT_DS_THETA_RAD, str);
  str << "(radians) ";
  for (int vp=0;vp<=ft.ySize()+1;vp++) str << theta_points_rad[vp] << ", ";
  str << theta_points_rad[ft.ySize()+2] << "\n";

  Params::setFormat(FORMAT_DSQUAD_SA, str);

  switch (code) {
    case DsQuad::SA : str << "Solid Angles:\n"; break;
    case DsQuad::SACT : str << "Solid Angle Abs Cos Theta:\n"; break;
    case DsQuad::CT : str << "Mean Cos Theta:\n"; break;
  }

  str << "Top Cap: "; 
  quad_array[0].display(str, code);
  str << "\n";

  str << "Bottom Cap ";
  quad_array[units()-1].display(str, code);
  str << "\n";

  str << "Quad Table (rows are horizontal, columns are vertical):\n";

  for (int vp=1;vp<=ft.ySize();vp++) {
    for (int hp=0;hp<ft.xSize();hp++) {
      int index = positionToIndex(vp,hp);
      quad_array[index].display(str, code);
      str << " ";
    }
    str << "\n";
  }
}
*/

