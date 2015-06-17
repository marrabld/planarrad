
// raddirecimp.h

#ifndef _JUDE_RAD_RADDIRECIMP_H
#define _JUDE_RAD_RADDIRECIMP_H

#include "raddirec.h"
#include "S3/vec3d.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "rad/raddirecdirecimp.h"
#include "rad/rd_matrixdft.h"
#include "ds/spherepartition.h"

using namespace jude::base;
using namespace jude::S3;

namespace jude {
namespace rad {



// formerly SpecImp<SD>

/*! \brief Base class for specific directional implementations of the RadImp Interface.

  Type A must always be a derived class of SBandData_Base.
*/

template <typename A>
class RadDirecImp : public RadDirec { 

public:

    /*! Constructor for unspecifed data type. */
  RadDirecImp<A>() { }
    /*! Constructor for unspecifed data type. */
  RadDirecImp<A>(int bc) : RadDirec(bc) { }
    /*! Constructor which sets the specifed data type to \a d. */
  RadDirecImp<A>(int bc, datatype d) : RadDirec(bc, d) { }
    /*! Constructor which copies the data type of \a rd. */
  RadDirecImp<A>(const RadDirecImp<A>& rd) : RadDirec(rd) { }

    /*! \brief Implementations must provide a function which creates a copy of structure and data.*/
  virtual RadDirecImp<A>* duplicate() const = 0;
    /*! \brief Implementations must provide a function which creates a copy of structure only, data undefined. */
  virtual RadDirecImp<A>* duplicateStructure() const = 0;
    /*! \brief Implementations must provide a function which creates a copy of structure and the data type only, data undefined. */
  virtual RadDirecImp<A>* duplicateStructureAndDataType() const = 0;
     /*! \brief Implementations must provide a function which creates a half version of the structure and copies data type, data undefined. */
  virtual RadDirecImp<A>* duplicateHalfStructureAndDataType() const = 0;
    /*! \brief Implementations must provide a function which creates a table with this objects directional structure. */
  virtual RadDirecDirecImp<A>* createDirecDirec() const = 0;

  virtual void copyFromMatrixDFT(const RD_MatrixDFT& rd_mat) = 0;
  virtual void copyToMatrixDFT(RD_MatrixDFT& rd_mat) const = 0;

  virtual void makeAll(const A& sd) = 0;
  virtual void makeAllZPlus(const A& sd) = 0;
  virtual void makeAllZMinus(const A& sd) = 0;
  virtual void makeZero() = 0;

  virtual void multiplyAll(const A& m) = 0;
  virtual void divideAll(const A& m) = 0;
 
    // the non-const version should be depreceated
  virtual A& at(const Vec3D& out_direc) = 0;
  virtual const A& at(const Vec3D& out_direc) const = 0;

    // the non-const version should be depreceated
  virtual A& atIndex(int i) = 0;
  virtual const A& atIndex(int i) const = 0;

    // this is how data should be written 
  virtual void setData(int i, const A& sd) = 0;
  virtual void setData(const Vec3D& direc, const A& sd) = 0;

  //virtual const A interpolated(const Vec3D& out_direc) const = 0;

  virtual const A& sumOfEntries() const = 0;
  virtual const A sumBySolidAngle() const = 0;
  virtual const A sumBySolidAngleAbsCosTheta() const = 0;
  virtual const A sumByAbsCosTheta() const = 0;
  virtual void sumBySolidAngle(A& zp, A& zm) const = 0;
  virtual void sumBySolidAngleAbsCosTheta(A& zp, A& zm) const = 0;

  void downsample(const RadDirecImp<A>& src); 

  const A planarIrradiance() const;

  const A scalarIrradiance() const;

    /*! \brief Returns the data in the z-plus end cap of \s in \sd or an averaged value around z-plus if the directional structure has no end cap. */
  const A atZPlus() const;
    /*! \brief Returns the data in the z-minus end cap of \s in \sd or an averaged value around z-minus if the directional structure has no end cap. */
  const A atZMinus() const;

  //bool azimuthallyAverage();
 
  virtual void addToData(int rotate_pos, const RadDirecImp<A>& out_data) = 0;
 
  void splitNegate(RadDirecImp<A>* zp, RadDirecImp<A>* zm) const;
  void splitRotateAboutX(RadDirecImp<A>* zp, RadDirecImp<A>* zm) const;

  void mergeNegate(const RadDirecImp<A>* zp, const RadDirecImp<A>* zm);
  void mergeRotateAboutX(const RadDirecImp<A>* zp, const RadDirecImp<A>* zm);


  //SBandData sd(emis->sumBySolidAngleAbsCosTheta());

 
  /*
  virtual SD integrateSurfaceRadiantPPU() const = 0;
 
  virtual void sumBySolidAngle(SD& zp, SD& zm) const = 0;
  virtual void sumBySolidAngleAbsCosTheta(SD& zp, SD& zm) const = 0;
  */

  //virtual void splitCreate(SpecImp<SD>*& f0, SpecImp<SD>*& f1) const { }

  //void split(RadDirecImp<A>* f0, RadDirecImp<A>* f1) const;
  //void merge(const RadDirecImp<A>* f0, const RadDirecImp<A>* f1);

  //const A topRadiance(double& sa) const;

  void display(JTextStream& str) const;
};



template <typename A>
const A RadDirecImp<A>::planarIrradiance() const {
  switch (data_type) {
    case MEAN_RAD : return sumBySolidAngleAbsCosTheta();
    case PLANAR_IRRAD : return sumOfEntries();
    case SCALAR_IRRAD : { }
    case UNSPECIFIED : { }
  }
  jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " data type is unspecified or of wrong type.\n";
  internalError();
  return A(0);
}



template <typename A>
const A RadDirecImp<A>::scalarIrradiance() const {
  switch (data_type) {
    case MEAN_RAD : return sumBySolidAngle();
    case PLANAR_IRRAD : { }
    // case PLANAR_IRRAD : return sumDivAbsCosTheta();
    case SCALAR_IRRAD : return sumOfEntries();
    case UNSPECIFIED : { }
  }
  jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " data type is unspecified or of wrong type.\n";
  internalError();
  return A(0);
}





/* \todo Implementation ought to be fixed a bit to ensure code is scalable - defer to DirecStruct derived class. */ 

template <typename A>
const A RadDirecImp<A>::atZPlus() const {

  const DirecStruct& ds(direcStruct());

  if (ds.isSpherePartition()) return atIndex(0);
  
  int index0 = ds.direcToIndex(Vec3D(0.001,0.001,1));
  int index1 = ds.direcToIndex(Vec3D(-0.001,0.001,1));
  int index2 = ds.direcToIndex(Vec3D(0.001,-0.001,1));
  int index3 = ds.direcToIndex(Vec3D(-0.001,-0.001,1));
  
  A sd = atIndex(index0);
  sd += atIndex(index1);
  sd += atIndex(index2);
  sd += atIndex(index3); 
  sd.divide(4);

  return sd;
}



/* \todo Implementation ought to be fixed a bit to ensure code is scalable - defer to DirecStruct derived class. */ 

template <typename A>
const A RadDirecImp<A>::atZMinus() const {

  const DirecStruct& ds(direcStruct());

  if (!ds.isFull()) { jlog::es << __FUNCTION__ << " called on non-full partition.\n"; internalError(); }

  //if (ds.isSpherePartition()) return atIndex(ds.units()-1);
  if (ds.isSpherePartition()) return at(Vec3D(0,0,-1));
  
  int index0 = ds.direcToIndex(Vec3D(0.001,0.001,-1));
  int index1 = ds.direcToIndex(Vec3D(-0.001,0.001,-1));
  int index2 = ds.direcToIndex(Vec3D(0.001,-0.001,-1));
  int index3 = ds.direcToIndex(Vec3D(-0.001,-0.001,-1));
  
  A sd = atIndex(index0);
  sd += atIndex(index1);
  sd += atIndex(index2);
  sd += atIndex(index3); 
  sd.divide(4);

  return sd;
}



/* \todo Implementation ought to be fixed a bit to ensure code is scalable - defer to DirecStruct derived class. */ 
/*
template <typename A>
bool RadDirecImp<A>::azimuthallyAverage() {

  const SpherePartition* ds = dynamic_cast<const SpherePartition*>(&direcStruct());

  if (!ds) return false;
  
  for (int v=0; v<ds->vertSize(); v++) {

    A sd(bandCount(), 0);

    for (int h=0; h<ds->horizSize(); h++) sd += atIndex(v*ds->horizSize() + h + 1);

    sd.divide(ds->horizSize());

    for (int h=0; h<ds->horizSize(); h++) atIndex(v*ds->horizSize() + h + 1) = sd;
  }

  return true;
}
*/



/*! Resamples the directional resolution data \a src to into this directional resolution data object.

This version is best to use when \a src is at higher directional resolution than this object. The directional quads 
of \src are projected onto this objects quads, so if \src quads are small then the resampling error is also small.
*/

template <typename A>
void RadDirecImp<A>::downsample(const RadDirecImp<A>& src) {

  if (!src.dataTypeIsMeanRad()) {
    jlog::es << "Internal Error: " << __FUNCTION__ << " not implemented for solid angle integral types\n";
    internalError();
  }

  const DirecStruct& src_ds = src.direcStruct();
  const DirecStruct& targ_ds = direcStruct();

  makeAll(0);

  double sa_sum[targ_ds.units()];
  for (int i=0; i<targ_ds.units(); i++) sa_sum[i] = 0;
 
  for (int i=0; i<src_ds.units(); i++) {

    Vec3D dv(src_ds.indexToMeanDirec(i));

    int targ_index = targ_ds.direcToIndex(dv);

    atIndex(targ_index) += src.atIndex(i) * src_ds.solidAngle(i);
    sa_sum[targ_index] += src_ds.solidAngle(i);
  }

  for (int i=0; i<targ_ds.units(); i++) atIndex(i) /= sa_sum[i];

  copyDataType(src);
}



/*! \brief Distributes the upward (z+) and downward (z-) data into \a f0 and \a f1 respectively using vector negation for (z-).

Does not make any assumption about equivalent directional structures since \a f0 and \a f1 are accessed by vector.
However data is written not avaeraged so serious aliasing errors will occur if directional structures do not match.
The access vector is negated to get the z- data into \a f1. This method of splitting full spherical directional data 
is only required since the plus-to-minus function cannot be DFT reduced if splitting by rotation about the x-axis is used.
This method of splitting functions should never be exposed in input or output data, since it becoems a left-handed 
co-ordinate system. The function splitRotateAboutX() is to be preferred in a non DFT use context. 
*/

template <typename A>
void RadDirecImp<A>::splitNegate(RadDirecImp<A>* zp, RadDirecImp<A>* zm) const {

  if (zp) { zp->makeZero(); zp->copyDataType(*this); }
  if (zm) { zm->makeZero(); zm->copyDataType(*this); }

     // loop through indices
  for (int i=0; i<direcStruct().units(); i++) {

    const A& sd = atIndex(i);
    
    const Vec3D dv(direcStruct().indexToPointDirec(i));

    // f0 is the function for photon travel in z- direction 
    
    if (dv.z() >= 0) { if (zp) zp->setData(dv, sd); }
    else { if (zm) zm->setData(-dv, sd); }
  }
}



/*! \brief Distributes the upward (z+) and downward (z-) data into \a f0 and \a f1 respectively using rotation about the x-axis for (z-).

Does not make any assumption about equivalent directional structures since \a f0 and \a f1 are accessed by vector.
However data is written not avaeraged so serious aliasing errors will occur if directional structures do not match.
The access vector is rotated 180 degrees around the x-direction axes to get the z- data into \a f1.
*/

template <typename A>
void RadDirecImp<A>::splitRotateAboutX(RadDirecImp<A>* zp, RadDirecImp<A>* zm) const {

  if (zp) { zp->makeZero(); zp->copyDataType(*this); }
  if (zm) { zm->makeZero(); zm->copyDataType(*this); }

     // loop through indices
  for (int i=0; i<direcStruct().units(); i++) {

    const A& sd = atIndex(i);
    const Vec3D dv(direcStruct().indexToMeanDirec(i));

    // f0 is the function for photon travel in z- direction 
    
    if (dv.z() >= 0) { if (zp) zp->setData(dv, sd); }
    else { if (zm) zm->setData(dv.rotate180AboutX(), sd); }
  }
}



/*! Combines the directional data of \a f0 and \a f1 into this object as the z+ and z- halves.*/ 

template <typename A>
void RadDirecImp<A>::mergeNegate(const RadDirecImp<A>* zp, const RadDirecImp<A>* zm) {

  makeZero();
 
     // loop through indices
  for (int i=0; i<direcStruct().units(); i++) {

    const Vec3D dv(direcStruct().indexToMeanDirec(i));
	
    // f0 is the function for photon travel in z- direction 
    
    if (dv.z() >= 0) { if (zp) setData(dv, zp->at(dv)); }
    else { if (zm) setData(dv, zm->at(-dv)); }
  }
}



/*! Combines the directional data of \a f0 and \a f1 into this object as the z+ and z- halves.*/ 

template <typename A>
void RadDirecImp<A>::mergeRotateAboutX(const RadDirecImp<A>* zp, const RadDirecImp<A>* zm) {

  makeZero();
 
     // loop through indices
  for (int i=0; i<direcStruct().units(); i++) {

    const Vec3D dv(direcStruct().indexToPointDirec(i));
	
    // f0 is the function for photon travel in z- direction 
    
    if (dv.z() >= 0) { if (zp) setData(dv, zp->at(dv)); }
    else { if (zm) setData(dv, zm->at(dv.rotate180AboutX())); }
  }
}



/*
template <typename A>
const A RadDirecImp<A>::topRadiance(double& sa) const {

  const DirecStruct& ds(direcStruct());

  if (ds.isSpherePartition()) {
    sa = ds.solidAngle(0);
    return atIndex(0);
  }

  int index0 = ds.direcToIndex(Vec3D(0.001,0.001,1));
  int index1 = ds.direcToIndex(Vec3D(-0.001,0.001,1));
  int index2 = ds.direcToIndex(Vec3D(0.001,-0.001,1));
  int index3 = ds.direcToIndex(Vec3D(-0.001,-0.001,1));

  sa = ds.solidAngle(index0) + ds.solidAngle(index1) + ds.solidAngle(index2) + ds.solidAngle(index3);

  A sd(atIndex(index0) + atIndex(index1) + atIndex(index2) + atIndex(index3)); 
  sd.divide(4);

  return sd;
}
*/



/*! \brief Interface for classes that retrieve info to display in text output of a directional data table. */
 
template <typename A> 
class RadDirecQDS : public QuadDataSource {

  const RadDirecImp<A>& rd;
  int band_index;

public:

  RadDirecQDS(const RadDirecImp<A>& rd, int bi) : 
    rd(rd), 
    band_index(bi) { }

  void displayAtIndex(JTextStream& str, int out_index, const JString& sub_sep) const {
    A::policy_type::logWrite(str, rd.atIndex(out_index).bandValue(band_index), sub_sep);
    //str << rd.atIndex(out_index).bandValue(band_index);
  }
};




/*! \brief Interface for classes that retrieve info to display in text output of a directional data table. */
 
template <typename A> 
class RadDirecQDW : public QuadDataWrite {

  RadDirecImp<A>& rd;
  int band_index;

public:

  RadDirecQDW(RadDirecImp<A>& rd, int bi) : 
    rd(rd), 
    band_index(bi) { }

  void setAtIndex(int index, double v) const {
    rd.atIndex(index).bandValue(band_index) = v;
  }
};



template <typename A>
void RadDirecImp<A>::display(JTextStream& str) const {
   
  for (int band_index=0; band_index<bandCount(); band_index++) {
    str << "Data in band index " << band_index << ":\n";
    RadDirecQDS<A> qds((*this), band_index);
    direcStruct().displayTableValues(str, qds, ", " , ",");
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECIMP_H

