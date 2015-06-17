
// direcstruct.h

#ifndef _JUDE_DS_DIRECSTRUCT_H
#define _JUDE_DS_DIRECSTRUCT_H

#include "S3/vec3d.h"
#include "base/jstring.h"
#include "base/jtexttable.h"

namespace jude { namespace base {
  class JDataStream;
  class JTextStream;
  class JParser;
} }

using namespace jude::base;
using namespace jude::S3;

namespace jude {
namespace ds {

class DsQuad;
class AdapBase;
class QuadDataSource;
class QuadDataWrite;


/*! \brief Interface for a directional partitioning of the sphere or hemisphere */

class DirecStruct {

  //virtual int typeCode() const = 0;

public:

  enum { LAMB=1, 
	 HEMISPHERE_VxH, HEMI_2x8, HEMI_4x16, HEMI_8x32, HEMI_16x64, 
	 SPHERE_VxH, SPHERE_4x8, SPHERE_8x16, SPHERE_16x32, SPHERE_32x64,
	 HEMICUBE_N, CUBE_N, DS_MAX };

  virtual ~DirecStruct() { }

  /*! \brief The subclass should create a copy of itself. */
  virtual DirecStruct* duplicate() const = 0;

  virtual const JString factoryID() const = 0;

  //virtual DirecStruct* createHalf() const { return 0; }

  //virtual int type() const = 0;
 
  virtual bool isFull() const = 0;
  virtual bool isSpherePartition() const = 0;
  virtual bool isCubePartition() const = 0;
  virtual double totalSolidAngle() const = 0;

  virtual int units() const = 0;
  virtual int unitsSquared() const = 0;

  virtual int direcToIndex(const Vec3D& out_direc) const = 0;

  virtual const Vec3D indexToMeanDirec(int i) const = 0;
  virtual const Vec3D indexToPointDirec(int i) const = 0;

  virtual int negatedIndex(int i) const = 0;

  //virtual void direcToInterpolated(const Vector3D& out_direc, DsQuadIpol& dqi) const = 0;

    /*! \brief Returns true if the z-value of quad \a i is greater than zero. */ 
  bool isZPlus(int i) const { return indexToPointDirec(i).z() > 0; }
    /*! \brief Returns true if the z-value of quad \a i is less than zero. */ 
  bool isZMinus(int i) const { return indexToPointDirec(i).z() < 0; }
 
    /*! \brief The mean cosine of zenith angle over the solid angular area of quad \a i. */
  virtual double meanCosTheta(int i) const = 0;
    /*! \brief The solid anglular area of quad \a i. */
  virtual double solidAngle(int i) const = 0;
    /*! \brief The mean cosine of zenith angle over the solid angular area of quad \a i multiplied by the solid anglular area of quad \a i. */
  virtual double solidAngleAbsCosTheta(int i) const = 0; 
  virtual double meanCubeDistance(int i) const = 0;
  virtual double pointCubeDistance(int i) const = 0;

    /*! \brief Returns a pointer to the quad corresponding to index \a i. */
  virtual DsQuad* quadAtIndex(int i) const { return 0; }
 
    /*! \brief Returns \c true if the two DirecStruct objects are of the same type (but not necessarily resolution). */
  virtual bool isSameType(const DirecStruct& ds) const = 0;
    /*! \brief Returns \c true if the two DirecStruct objects represent the same directional partition. */
  virtual bool isEqual(const DirecStruct& ds) const = 0;

  //virtual int nonAdapIndexNum() const = 0;
  //virtual int nonAdapIndex(int i) const = 0;

  //virtual int adapTopNodes() const = 0;
  //virtual int adapTopNodesSQ() const = 0;
 
  //virtual const AdapBase& indexToAdapBase(int index) const = 0;
  
  //virtual int adaptiveIndexToTable(int index, int &vp, int &hp) const = 0;
  //virtual int adaptiveTableToIndex(int top_node, int v, int h) const = 0;
 
  //virtual int adapFirstXJump() const = 0;
  //virtual int adapFirstYJump() const = 0;

  //virtual int adaptiveMaxDepth() const = 0;
 
  //virtual double adaptiveSolidAngle(int index, int depth) const = 0;
 
  //virtual int adaptiveVertNum() const = 0;
  //virtual int adaptiveHorizNum() const = 0;
  //virtual int adaptiveVertBits() const = 0;
  //virtual int adaptiveHorizBits() const = 0;
 
  virtual Vec3D randDirecFromIndex(int i) const { return Vec3D(1,0,0); }

    /*! \brief Returns the number of equivalent azimuthally rotated positions. */
  virtual int azimuthallyRotatedPositionCount() const = 0;
    /*! \brief Returns the equivent index to \a index mapped to the azimuthally rotated position \a pos, where pos ranges from 0 to azimuthallyRotatedPositionCount()-1. */ 
  virtual int azimuthallyRotatedIndex(int rotated_pos, int index) const = 0;

  virtual int symmetricalTransNum() const = 0;
  virtual const int* symmetricalIndices(int i) const = 0;

  virtual void symmetricalIndicesConvert(int& in_index, int& out_index) const { }
  virtual int symmetricalNodesRepresented(int in_index, int out_index) const { return 1; }
 
  virtual void read(JDataStream& str) = 0;
  virtual void write(JDataStream& str) const = 0;
  //virtual void read(JTextStream& str) = 0;
  //virtual void write(JTextStream& str) const = 0;

  virtual void displayTableValues(JTextStream& str, int code, const JString& major_sep) const = 0;
  virtual void displayTableValues(JTextStream& str, const QuadDataSource& qds, const JString& major_sep, const JString& sub_sep) const { }

  virtual void readTableValues(const JTextTable& ttab, int start_row, int start_col, const QuadDataWrite& qdw) const { }

  void display() const;
  virtual void display(JTextStream& str, int code, const JString& major_sep) const { }

 
  //virtual const JString typeText() const { return JString::null; }

  //virtual void collectInfo(JParser& prm, const JString& prefix) const { };
};



} } // namespace jude::ds

#endif // _JUDE_DS_DIRECSTRUCT_H

