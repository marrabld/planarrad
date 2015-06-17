
// hemispherepartvxh.h

#ifndef _JUDE_DS_HEMISPHEREPARTVXH_H
#define _JUDE_DS_HEMISPHEREPARTVXH_H

// this include should be eventually removable as all DirecStrcut derived classes 
// should be inlcuded via direcstructtypes.h which puts the trait classes first
//#include "direcstructtypes.h"

#include "spherepartition.h"
#include "hsparttable.h"
//#include "globalfuncs.h"
//#include "addresscoder.h"
#include "base/jlist.h"

namespace jude {
namespace ds {

class SpherePartVxH;
class DsQuadIpol;



// HemiSpherePartVxH
// this implements a general quad partitioning of the hemisphere
// of (vn) rows by (hn) columns plus an end cap

class HemiSpherePartVxH : public SpherePartition {

  HSPartTable* tab;

  static class TableList : public JList<HSPartTable> {
  public:
    ~TableList();
  } table_list;

  void findOrAddTable(int vn, int hn, const std::vector<double>* tp);

public:

  //static void dumpWriteTableList(DumpStreamOut& str) { GlobalFuncs::dumpWriteListMembers(table_list, str); }
  //static void dumpBuildTableList(DumpStreamIn& str) { GlobalFuncs::dumpBuildListMembers(table_list, str); }

    // the type for a hemi-spherical representaion of this partition type - is just this type
  typedef HemiSpherePartVxH half_type;
  typedef SpherePartVxH full_type;

  HemiSpherePartVxH() : tab(0) { }
  HemiSpherePartVxH(int vn, int hn, const std::vector<double>* tp=0) { findOrAddTable(vn,hn,tp); }
  HemiSpherePartVxH(int vn, int hn, const std::vector<double>& tp) { findOrAddTable(vn,hn,&tp); }
  HemiSpherePartVxH(JDataStream& str);
  HemiSpherePartVxH(JTextStream& str);
  HemiSpherePartVxH(const DirecStruct& ds);
  HemiSpherePartVxH(const HemiSpherePartVxH& ds) : tab(ds.tab) { }
  HemiSpherePartVxH(const SpherePartVxH& ds);

  HemiSpherePartVxH* duplicate() const { return new HemiSpherePartVxH(*this); }
    // returns a hemispherical version of this partition type - is just this partition type
  const HemiSpherePartVxH half() const { return HemiSpherePartVxH(*this); }
    // returns a full version of this partition type
  const SpherePartVxH* createFull() const;

    /*! Returns the factory ID needed to reconstruct this object on loading. */
  virtual const JString factoryID() const { return staticFactoryID(); }
    /*! Version of factoryID() that does not reuire an object to be called. */
  static const JString staticFactoryID() { return "hspvh"; }

    /*! Returns false as this is not a full spherical directional structure. */
  bool isFull() const { return false; }
    /*! Version of isFull() that does not reuire an object to be called. */
  static bool isFullStatic() { return false; }
    /*! Returns the total solid angle of this directional stucture, 2PI. */
  double totalSolidAngle() const { return 2*M_PI; }
 
  // these functions are specific to spherical directional structures
  // and most are only used by discrete Fourier transfrom routines
 
  int vertSize() const { return tab->vertSize(); }
  int horizSize() const { return tab->horizSize(); }

  bool isRegular() const { return tab->isRegular(); }
  const std::vector<double>& thetaPointsDeg() const { return tab->thetaPointsDeg(); }
  const std::vector<double>& thetaPointsRad() const { return tab->thetaPointsRad(); }
 
  int positionToIndex(int vp, int hp) const { return tab->positionToIndex(vp,hp); }

    // used for DFT representations
  int positionToIndexDFT(int vp, int hp) const { return tab->positionToIndex(vp,hp); }
  void indexToPositionDFT(int index, int& vp, int& hp) const { tab->indexToPosition(index, vp, hp); }
  double epsilonDFT(int lm) const { return (lm==0 || lm==horizSize()/2) ? horizSize() : horizSize()/2; }
  double gammaDFT(int lm) const { return (lm==0 || lm==horizSize()/2) ? 0 : horizSize()/2; }
  int rowsDFT() const { return vertSize() + 1; }
  int lmNumDFT() const { return (horizSize() / 2) + 1; }

  //void setSize(int vn, int hn) { findOrAddTable(vn,hn); }
 
  int units() const { return tab->units(); }
  int unitsSquared() const { return tab->unitsSquared(); }

  int direcToIndex(const Vec3D& out_direc) const { return tab->direcToIndex(out_direc); }

  const Vec3D indexToMeanDirec(int i) const { return tab->indexToMeanDirec(i); }
  const Vec3D indexToPointDirec(int i) const { return tab->indexToPointDirec(i); }

  int negatedIndex(int) const { return -1; } // shouldn't call

  void direcToInterpolated(const Vec3D& out_direc, DsQuadIpol& dqi) const { tab->direcToInterpolated(out_direc, dqi); }

  double meanCosTheta(int i) const { return tab->meanCosTheta(i); }
  double solidAngle(int i) const { return tab->solidAngle(i); }
  double solidAngleAbsCosTheta(int i) const { return tab->solidAngleAbsCosTheta(i); }
  double meanCubeDistance(int i) const { return tab->meanCubeDistance(i); }
  double pointCubeDistance(int i) const { return tab->pointCubeDistance(i); }
 
  DsQuad* quadAtIndex(int i) const { return tab->quadAtIndex(i); }
  DsQuad* adapQuadAtIndex(int d, int i) const { return tab->adapQuadAtIndex(d,i); }
 
  Vec3D randDirecFromIndex(int i) const { return tab->randDirecFromIndex(i); }
 
  bool isSameType(const DirecStruct& ds) const;
  bool isEqual(const DirecStruct& ds) const;

  int nonAdapIndexNum() const { return 1; }
  int nonAdapIndex(int) const { return 0; }

  int adapTopNodes() const { return 1; }
  int adapTopNodesSQ() const { return 1; }
 
  /*
  const AdapBase& indexToAdapBase(int index) const { return tab->indexToAdapBase(index); }
  int adapJointNodeNum(const AdapBase& ab_in, const AdapBase& ab_out) const { return tab->adapJointNodeNum(ab_in, ab_out); }

  const AdapBase& adapBase(int n) const { return tab->adapBase(n); }
  const AdapBase& adapBaseIn(int n) const { return tab->adapBaseIn(n); }
  const AdapBase& adapBaseOut(int n) const { return tab->adapBaseOut(n); }
 
  int adapFirstXJump() const { return tab->adapFirstXJump(); }
  int adapFirstYJump() const { return tab->adapFirstYJump(); } 
  int adapMaxDepth() const { return tab->adapMaxDepth(); }
 
  double adapSolidAngle(int depth, int index) const { return tab->adapQuadAtIndex(depth,index)->solidAngle(); }
  double adapSolidAngleAbsCosTheta(int depth, int index) const { return tab->adapQuadAtIndex(depth,index)->solidAngleAbsCosTheta(); }
  */

    /*! Returns the number of equivalent azimuthally rotated positions. */
  int azimuthallyRotatedPositionCount() const { return tab->azimuthallyRotatedPositionCount(); }
    /*! Returns the equivent index to \a index mapped to the azimuthally rotated position \a pos, where pos ranges from 0 to azimuthallyRotatedPositionCount()-1. */ 
  int azimuthallyRotatedIndex(int rotated_pos, int index) const { return tab->azimuthallyRotatedIndex(rotated_pos, index); }

  int symmetricalTransNum() const { return tab->symmetricalTransNum(); }
  const int* symmetricalIndices(int trans_set) const { return tab->symmetricalIndices(trans_set); }

  void read(JDataStream& str);
  void write(JDataStream& str) const;

  // depreceated
  //void read(JTextStream& str);
  //void write(JTextStream& str) const;

  //const JString typeText() const { 
  //  JString s("HemiSpherePartVxH (");
  // s.append(JString::number(vertSize())).append(",").append(JString::number(horizSize()));
  //  return s.append(")"); 
  // }

  //void writeToDataStream(JDataStream& str) const;
  //void readFromDataStream(JDataStream& str); 
  //void collectInfo(JParser& prm, const JString& prefix) const;
};



} } // namespace jude::ds

#endif // _JUDE_DS_HEMISPHEREPARTVXH_H

