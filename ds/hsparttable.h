
// hsparttable.h

#ifndef _JUDE_DS_HSPARTTABLE_H
#define _JUDE_DS_HSPARTTABLE_H

#include "parttable.h"
#include "spquad.h"
#include "base/jindextable.h"
#include "base/jlist.h"
#include "base/jdatastream.h"
#include "S3/vec3d.h"
#include <vector>


using namespace jude::base;

namespace jude {
namespace ds {

typedef JIndexTable TableType;

// fasttables do not seem to exist anymore

//#ifdef FASTTABLE
//#include "fasttable.h"
//typedef FastTable TableType;
//#else
//#endif

class HemiSpherePartVxH;
class DsQuadIpol;



/*! \brief Table of hemishperical partion quad details. */

class HSPartTable : public PartTable {

  TableType ft;

  double horiz_angle;
  double vert_angle;   // only if regular otherwise = 0
  double cap_angle;
  
  bool is_regular;

  std::vector<double> theta_points_deg;
  std::vector<double> theta_points_rad;

  SpQuad* quad_array;

  JList<SpQuad> all_quads_list;
  SpQuad** adap_array;


  void buildTable();
  
  //double thetaAngularWidth() const { return vert_angle; }
  double phiAngularWidth() const { return horiz_angle; }

    // vp starts at 0 for first row under cap
  double thetaMinRad(int vp) const { return (isRegular()) ? ((double)vp) * vert_angle + cap_angle : theta_points_rad[vp+1]; }
  double thetaMaxRad(int vp) const { return (isRegular()) ? ((double)vp+1) * vert_angle + cap_angle : theta_points_rad[vp+2]; }

  double proportionalAreaCalc(int i) const;
  const Vec3D indexToDirecCalc(int i) const;

  int direcToIndex_NonRegular(const Vec3D& out_direc) const;
  int indexFromPos(double hpos, double vpos) const;
  
public:
 
  //static void dumpWrite(const HSPartTable* tab, DumpStreamOut& str) { 
  //  tab->write(str); 
  //  str << (void*) tab; 
  // }

  //static HSPartTable* dumpCreate(DumpStreamIn& str) { return str.assocAddress(new HSPartTable(str)); }

  HSPartTable(int vn, int hn) : 
    ft(hn,vn),
    is_regular(true),
    adap_array(0) { buildTable(); }

  HSPartTable(int vn, int hn, const std::vector<double>& tp_deg);

  HSPartTable(JDataStream& str) : 
    ft(str),
    adap_array(0) { buildTable(); }

  ~HSPartTable() { 
    delete[] quad_array;
    delete[] adap_array;
  }
 
  void write(jude::base::JDataStream& str) const { ft.write(str); }
 
  int vertSize() const { return ft.ySize(); }
  int horizSize() const { return ft.xSize(); }

  bool isRegular() const { return is_regular; }
  const std::vector<double>& thetaPointsDeg() const { return theta_points_deg; }
  const std::vector<double>& thetaPointsRad() const { return theta_points_rad; }

    // vp = 0 top cap
  int positionToIndex(int vp, int hp) const { return (vp==0) ? 0 : ft.positionToIndex(hp,vp-1) + 1; }

  void indexToPosition(int index, int& vp, int& hp) const {
    if (index==0) { vp = hp = 0; return; }
    ft.indexToPosition(index-1, hp, vp);
    vp++;
  }

  int units() const { return ft.totalSize() + 1; }
  int unitsSquared() const { return units() * units(); }

  int direcToIndex(const Vec3D& out_direc) const;

  const Vec3D indexToMeanDirec(int i) const { return quad_array[i].meanDirec(); }
  const Vec3D indexToPointDirec(int i) const { return quad_array[i].pointDirec(); }

  void direcToInterpolated(const Vec3D& out_direc, DsQuadIpol& dqi) const;

  double meanCosTheta(int i) const { return quad_array[i].meanCosTheta(); }
  double solidAngle(int i) const { return quad_array[i].solidAngle(); }
  double solidAngleAbsCosTheta(int i) const { return quad_array[i].solidAngleAbsCosTheta(); }
  double meanCubeDistance(int i) const { return quad_array[i].meanCubeDistance(); }
  double pointCubeDistance(int i) const { return quad_array[i].pointCubeDistance(); }

  Vec3D randDirecFromIndex(int i) const { return quad_array[i].randomDirec(); }
 
  SpQuad* quadAtIndex(int i) const { return &quad_array[i]; }
 
  SpQuad* adapQuadAtIndex(int d, int i) const { return adap_array[d*units() + i]; }


    /*! Returns the number of equivalent azimuthally rotated positions. */
  int azimuthallyRotatedPositionCount() const { return horizSize(); }
    /*! Returns the equivent index to \a index mapped to the azimuthally rotated position \a pos, where pos ranges from 0 to azimuthallyRotatedPositionCount()-1. */ 
  int azimuthallyRotatedIndex(int rotated_pos, int index) const;

  int symmetricalTransNum() const { return 0; }
  const int* symmetricalIndices(int trans_set) const { return 0; }
};



} } // namespace jude::ds

#endif // _JUDE_DS_HSPARTTABLE_H

