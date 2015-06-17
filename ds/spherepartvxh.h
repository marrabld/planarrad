
// spherepartvxh.h

#ifndef _JUDE_DS_SPHEREPARTVXH_H
#define _JUDE_DS_SPHEREPARTVXH_H

#include "spherepartition.h"
#include "sparttable.h"
#include "hemispherepartvxh.h"
#include "base/jlist.h"

namespace jude {
namespace ds {



/*! \brief A general quad partitioning of the full sphere based on lines of constant azimuth and zenith angle.

The partitioning consists of a table of \c v rows by \c h columns wrapped around the sphere, plus end caps at the top and bottom. 
Table rows correspond to quads of constant zenith angle, whereas columns are quads of constant azimuth angle.
*/

class SpherePartVxH : public SpherePartition {

  SPartTable* tab;

  static class TableList : public JList<SPartTable> {
  public:
    ~TableList();
  } table_list;

  void findOrAddTable(int vn, int hn, const std::vector<double>* tp);

public:

  //static void dumpWriteTableList(DumpStreamOut& str) { GlobalFuncs::dumpWriteListMembers(table_list, str); }
  //static void dumpBuildTableList(DumpStreamIn& str) { GlobalFuncs::dumpBuildListMembers(table_list, str); }

    // the type for a hemi-spherical representaion of this partition type
  typedef HemiSpherePartVxH half_type;

  SpherePartVxH() : tab(0) { }
  SpherePartVxH(int vn, int hn, const std::vector<double>* tp=0) { findOrAddTable(vn,hn,tp); }
  SpherePartVxH(int vn, int hn, const std::vector<double>& tp) { findOrAddTable(vn,hn,&tp); }
  SpherePartVxH(JDataStream& str);
  SpherePartVxH(JTextStream& str);
  SpherePartVxH(const DirecStruct& ds);
  SpherePartVxH(const SpherePartVxH& ds) : tab(ds.tab) { }
  SpherePartVxH(const HemiSpherePartVxH& ds);
  ~SpherePartVxH();

    // assumes the table list has already been built
  //SpherePartVxH(DumpStreamIn& str) { 
  //  str >> (void*&) tab;
  //  str.coder().externalToLocal(tab);
  // }

  //void dumpWrite(DumpStreamOut& str) const { str << (void*) tab; }

  SpherePartVxH* duplicate() const { return new SpherePartVxH(*this); }
    // returns a hemispherical version of this partition type
  const HemiSpherePartVxH half() const { return HemiSpherePartVxH(*this); }
    // returns a full spherical version of this partition type - is just this partition type
  const SpherePartVxH* createFull() const { return new SpherePartVxH(*this); }
 
  //HemiSpherePartVxH half() const { return HemiSpherePartVxH(*this); }

  //int type() const { return DirecStructType<SpherePartVxH>::id(); }

  //int typeCode() const { return DirecStructType<SpherePartVxH>::id(); }
  //static int typeCodeStatic() { return DirecStructType<SpherePartVxH>::id(); }

    // the factory ID needed to reconstruct this object on loading
  virtual const JString factoryID() const { return staticFactoryID(); }
    // version that doesn't need an object
  static const JString staticFactoryID() { return "spvh"; }

  bool isFull() const { return true; }
  double totalSolidAngle() const { return 4*M_PI; }
 
  static bool isFullStatic() { return true; }
 
  // these functions are specific to spherical directional structures
  // and most are only used by discrete Fourier transfrom routines
 
  int vertSize() const { return tab->vertSize(); }
  int horizSize() const { return tab->horizSize(); }

  bool isRegular() const { return tab->isRegular(); }
  const std::vector<double>& thetaPointsDeg() const { return tab->thetaPointsDeg(); }
  const std::vector<double>& thetaPointsRad() const { return tab->thetaPointsRad(); }

  int positionToIndex(int vp, int hp) const { return tab->positionToIndex(vp,hp); }

    // why the DFT ? depreceated ?
  int positionToIndexDFT(int vp, int hp) const { return tab->positionToIndex(vp,hp); }
  void indexToPositionDFT(int index, int& vp, int& hp) const { tab->indexToPosition(index, vp, hp); }
  double epsilonDFT(int lm) const { return (lm==0 || lm==horizSize()/2) ? horizSize() : horizSize()/2; }
  double gammaDFT(int lm) const { return (lm==0 || lm==horizSize()/2) ? 0 : horizSize()/2; }
  int rowsDFT() const { return vertSize() + 2; }
  int lmNumDFT() const { return (horizSize() / 2) + 1; }
 
  //void setSize(int vn, int hn) { findOrAddTable(vn,hn); }

  int units() const { return tab->units(); }
  int unitsSquared() const { return tab->unitsSquared(); }

  int direcToIndex(const Vec3D& out_direc) const { return tab->direcToIndex(out_direc); }

  const Vec3D indexToMeanDirec(int i) const { return tab->indexToMeanDirec(i); }
  const Vec3D indexToPointDirec(int i) const { return tab->indexToPointDirec(i); }

  int negatedIndex(int i) const { return tab->negatedIndex(i); }

  void direcToInterpolated(const Vec3D& out_direc, DsQuadIpol& dqi) const { return tab->direcToInterpolated(out_direc, dqi); }

  //double phi(int i) const { return tab->quadAtIndex(i)->phi(); }

  double meanCosTheta(int i) const { return tab->meanCosTheta(i); }
  double solidAngle(int i) const { return tab->solidAngle(i); }
  double solidAngleAbsCosTheta(int i) const { return fabs(tab->solidAngle(i)*tab->meanCosTheta(i)); }
  double meanCubeDistance(int i) const { return tab->meanCubeDistance(i); }
  double pointCubeDistance(int i) const { return tab->pointCubeDistance(i); }
 
  DsQuad* quadAtIndex(int i) const { return tab->quadAtIndex(i); }
  //DsQuad* adapQuadAtIndex(int d, int i) const { return tab->adapQuadAtIndex(d,i); }
 
  Vec3D randDirecFromIndex(int i) const { return tab->randDirecFromIndex(i); }
 
  bool isSameType(const DirecStruct& ds) const;
  bool isEqual(const DirecStruct& ds) const;
 
  /*
  int nonAdapIndexNum() const { return 2; }
  int nonAdapIndex(int i) const { return (i) ? units()-1 : 0; }
  
  int adapTopNodes() const { return 1; }
  int adapTopNodesSQ() const { return 1; }
  
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

  void symmetricalIndicesConvert(int& in_index, int& out_index) const { 
    return tab->symmetricalIndicesConvert(in_index, out_index);  
  }

  int symmetricalNodesRepresented(int in_index, int out_index) const { 
    return tab->symmetricalNodesRepresented(in_index, out_index); 
  }

  void read(JDataStream& str);
  void write(JDataStream& str) const;

  //void read(JTextStream& str);
  //void write(JTextStream& str) const;
 
  // depreceated
  //void writeToDataStream(JDataStream& str) const;
  //void readFromDataStream(JDataStream& str); 
  //void collectInfo(JParser& prm, const JString& prefix) const;
};



} } // namespace jude::ds

#endif // _JUDE_DS_SPHEREPARTVXH_H

