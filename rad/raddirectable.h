
// raddirectable.h

#ifndef _JUDE_RAD_RADDIRECTABLE_H
#define _JUDE_RAD_RADDIRECTABLE_H

#include "rad/raddirecimp.h"
#include "rad/raddirecdft.h"
#include "table/directable.h"
#include "base/jgenericfactory.h"
#include "ds/spherepartvxh.h"
#include "ds/hemispherepartvxh.h"
#include "rad/sbanddata.h"
#include "rad/raddirecdirectable.h"

using namespace jude::table;
using namespace jude::ds;

namespace jude {
namespace rad {



/*! A table with directional structure DS storing type SD which must be a derived class of SBandData_Base. */

template <typename A, typename DS>
class RadDirecTable : public RadDirecImp<A> {

  DirecTable<A, DS> dt;

  virtual const RadDirecTable<A, DS>* assertCompatible(const RadDirec& rhs, const char* func_name) const {
      // dynamic cast to check direct struct and data type polymorphism
    const RadDirecTable<A, DS>* rhs_cast(dynamic_cast<const RadDirecTable<A, DS>* >(&rhs));
      // message and internal error if polymorphism not compatible
    if (!rhs_cast) { jlog::ls << "Internal error " << func_name << " incompatible polymorphic types.\n"; internalError(); }
      // check that the directional partition structure is the same 
    if (!direcStruct().isEqual(rhs_cast->direcStruct())) {
      jlog::ls << "Internal error " << func_name << " differing directional structures.\n";
      internalError();
    }
    return rhs_cast;
  }

protected:

  DirecTable<A, DS>& tableData() { return dt; }
 
public:

  RadDirecTable() { }
  RadDirecTable(const DirecStruct& ds, int bc) : RadDirecImp<A>(bc), dt(ds) { }
  RadDirecTable(const DirecStruct& ds, int bc, RadDirec::datatype d) : RadDirecImp<A>(bc,d), dt(ds) { }
  RadDirecTable(const RadDirecTable<A,DS>& rhs) : RadDirecImp<A>(rhs), dt(rhs.dt) { }
  RadDirecTable(const RadDirecDFT<A,DS>& rhs) : RadDirecImp<A>(rhs.bandCount()), dt(rhs.direcStruct()) { copy(rhs); }

  RadDirecTable<A,DS>* duplicate() const { return new RadDirecTable<A,DS>(*this); }
  RadDirecTable<A,DS>* duplicateStructure() const { return new RadDirecTable<A,DS>(direcStruct(), RadDirec::bandCount()); }

  RadDirecTable<A,DS>* duplicateStructureAndDataType() const { 
    RadDirecTable<A,DS>* rd = new RadDirecTable<A,DS>(direcStruct(), RadDirec::bandCount()); 
    rd->copyDataType(*this);
    return rd;
  }

  RadDirecTable<A, typename DS::half_type>* duplicateHalfStructureAndDataType() const { 
    RadDirecTable<A, typename DS::half_type>* rd = new RadDirecTable<A, typename DS::half_type>(direcStruct().half(), RadDirec::bandCount()); 
    rd->copyDataType(*this);
    return rd;
  }

  RadDirecDirecTable<A,DS>* createDirecDirec() const { return new RadDirecDirecTable<A,DS>(direcStruct(), RadDirec::bandCount()); }

    /*! \brief Copy the data in the DFT representation \a rd_dft into this object. */
  void copy(const RadDirecDFT<A,DS>& rd_dft);

    /*! \brief Copy the data in the matrix-DFT representation \a rd_mat into this object. */
  void copyFromMatrixDFT(const RD_MatrixDFT& rd_mat);
    /*! \brief Convert the data in this into matrix-DFT representation and store in \a rd_mat. */
  void copyToMatrixDFT(RD_MatrixDFT& rd_mat) const;

    // the factory ID needed to reconstruct this object on loading
  virtual const JString factoryID() const { return staticFactoryID(); }
    // safe to assume A and DS are not virtual types
  static const JString staticFactoryID() { return "rdt_"+staticIdOf<A>()+"_"+staticIdOf<DS>(); }

    /*! \brief Returns the directional partition structure. */
  const DS& direcStruct() const { return dt.direcStruct(); }

  //uns32 memoryUse() const { return sizeof(DirecTable<A,DS>) + A::memoryUseOneBand() * RadDirec::bandCount() * direcStruct().units(); }

  const DirecTable<A, DS>& tableData() const { return dt; }

    /*! \brief Returns the number of cells or 'quads' in the directional partiton structure. */
  int units() const { return dt.units(); }
 
    /*! \brief Sets all tabulated entries to \a sd.*/
  void makeAll(const A& sd) { dt.initAll(sd); }
    /*! \brief Sets all tabulated entries to \a sd where directional z-value is greater than zero.*/
  void makeAllZPlus(const A& sd) { dt.initAllZPlus(sd); }
    /*! \brief Sets all tabulated entries to \a sd where directional z-value is less than zero.*/
  void makeAllZMinus(const A& sd) { dt.initAllZMinus(sd); }

    /*! \brief Zeros the data in all tabulated entries.*/
  void makeZero() { dt.initAll(A(RadDirec::bandCount(),0)); }

    /*! \brief Changes the band count to \a bc, filling the band data of each emtry with the current first band value. */
  void expandBandCount(int bc);

  void resample( const BandSpec& targ_bs, const BandSpec& src_bs);
 
  void lazyInvalidate() const { dt.lazyInvalidate(); }

    // the non-const version should be depreceated
  A& at(const Vec3D& out_direc) { return dt.at(out_direc); }
  const A& at(const Vec3D& out_direc) const { return dt.at(out_direc); }

    // the non-const version should be depreceated
  A& atIndex(int i) { return dt.atIndex(i); }
  const A& atIndex(int i) const { return dt.atIndex(i); }

    // this is how data should be written 
  void setData(int i, const A& sd) { dt.atIndex(i) = sd; }
  void setData(const Vec3D& direc, const A& sd) { dt.at(direc) = sd; }

  void divideAllByScalar(double f) { dt.divide(f); }

    // TODO these divide functions need clarifying
  void multiply(basic_radtype f) { dt.multiply(f); }
  void divide(basic_radtype f) { dt.divide(f); }

  void multiplyAll(const A& m) { dt.multiply(m); }
  void divideAll(const A& m) { dt.divide(m); }

  void copy(const RadDirecTable<A, DS>& r) { dt.copy(r.dt); RadDirecImp<A>::copy(r); }
  void add(const RadDirecTable<A, DS>& r) { dt.add(r.dt); RadDirecImp<A>::add(r); }
  void subtract(const RadDirecTable<A, DS>& r) { dt.subtract(r.dt); RadDirecImp<A>::subtract(r); }

  void addMultiple(const RadDirecTable<A, DS>& r, double m) { dt.addMultiple(r.dt, m); }

  void copy(const RadDirec& rhs) { copy(*assertCompatible(rhs, __FUNCTION__)); }
  void add(const RadDirec& rhs) { add(*assertCompatible(rhs, __FUNCTION__)); }
  void subtract(const RadDirec& rhs) { subtract(*assertCompatible(rhs, __FUNCTION__)); }
 
  void multiplyAllBySA() { dt.multiplyAllBySA(); RadDirec::changeDataType(RadDirec::MEAN_RAD, RadDirec::SCALAR_IRRAD); }
  void multiplyAllByCT()  { dt.multiplyAllByCT(); RadDirec::changeDataType(RadDirec::SCALAR_IRRAD, RadDirec::PLANAR_IRRAD); }
  void multiplyAllBySACT() { dt.multiplyAllBySACT(); RadDirec::changeDataType(RadDirec::MEAN_RAD, RadDirec::PLANAR_IRRAD); }

  // void divideAllBySA() = 0;
  void divideAllByCT() { dt.divideAllByCT(); RadDirec::changeDataType(RadDirec::PLANAR_IRRAD, RadDirec::SCALAR_IRRAD); }
  void divideAllBySACT() { dt.divideAllBySACT(); RadDirec::changeDataType(RadDirec::PLANAR_IRRAD, RadDirec::MEAN_RAD); }
 
    /*! \brief Returns the sum of entries. */
  const A& sumOfEntries() const { return dt.sumOfEntries(); } 
    /*! \brief Returns integral over solid angle of the directional function. */
  const A sumBySolidAngle() const { return dt.sumBySolidAngle(); }
    /*! \brief Returns integral over solid angle of the directional function multiplied by cos(theta). */
  const A sumBySolidAngleAbsCosTheta() const { return dt.sumBySolidAngleAbsCosTheta(); }
    /*! \brief Returns integral over solid angle of the directional function multiplied by cos(theta). */
  const A sumByAbsCosTheta() const { return dt.sumByAbsCosTheta(); }
 
    /*! \brief Returns integral over solid angle of the directional function separated into z-plus and z-minus hemispheres. */
  void sumBySolidAngle(A& zp, A& zm) const { dt.sumBySolidAngle(zp,zm); }
    /*! \brief Returns integral over solid angle of the directional function multiplied by cos(theta) separated into z-plus and z-minus hemispheres. */
  void sumBySolidAngleAbsCosTheta(A& zp, A& zm) const { dt.sumBySolidAngleAbsCosTheta(zp,zm); }

    /*! \brief Returns true if this object and \a rhs are of the same polymorphic type and have the same directional partition structure. */
  bool checkCompatible(const RadDirec& rhs) const;

  void splitCreateRotateAboutX(RadDirecTable<A, typename DS::half_type>*& f0, RadDirecTable<A, typename DS::half_type>*& f1) const;

  void addToData(int rotate_pos, const RadDirecImp<A>& out_data);

    /*! \brief Reads directional partition parameters and table values from a data stream, where source data was created by \a saved_version of the library. */
  bool read(JDataStream& str, uns32 saved_version) { 
      // read band count and data type
    if (!RadDirec::read(str, saved_version)) return false; 
      // read and set up dirctional structure first
    if (!dt.readDirecStruct(str, saved_version)) return false;
      // the new way of saving is that the band count is saved once within RadDirec
      // so after getting the directional structure have to initialise all data to the right number of bands
      //! \todo This oculd be done better with a policy class for loading of SBandData that contained the band count.
    if (saved_version >= VERSION_CODE(2, 1, 0)) { makeAll(A(RadDirec::bandCount(), typename A::elem_type(0))); }
      // read the data
    if (!dt.readData(str, saved_version)) return false;
      // In the old way of saving overall band count is not saved but each SbandData saved the band count
    if (saved_version < VERSION_CODE(2, 1, 0)) RadDirec::setBandCount(atIndex(0).bandCount());
    return true;
  }

  bool write(JDataStream& str) const { 
    if (!RadDirec::write(str)) return false; 
    return dt.write(str); 
  }

  bool read(JTextStream& str) { return false; }
  bool write(JTextStream& str) const { return false; }

  friend class RadDirecDirecTable<A,DS>;
};



// RadDirecTable<A,DS>::expandBandCount()

template <typename A, typename DS>
void RadDirecTable<A,DS>::expandBandCount(int bc) {
  A* ptr = dt.dataArray();
  const A* ptr_end = dt.dataArray() + dt.units();
  for (; ptr!=ptr_end; ptr++) (*ptr) = A(bc,(*ptr)[0]);
  RadDirec::setBandCount(bc);
}



// RadDirecTable<A,DS>::resample()

template <typename A, typename DS>
void RadDirecTable<A,DS>::resample(const BandSpec& targ_bs, const BandSpec& src_bs) {
  A* ptr = dt.dataArray();
  const A* ptr_end = dt.dataArray() + dt.units();
  for (; ptr!=ptr_end; ptr++) (*ptr) = targ_bs.resampleToThis(*ptr, src_bs);
  RadDirec::setBandCount(targ_bs.bandCount());
}



// RadDirecTable<A,DS>::checkCompatible()

template <typename A, typename DS>
bool RadDirecTable<A,DS>::checkCompatible(const RadDirec& rhs) const {
    // dynamic cast to check direct struct and data type polymorphism
  const RadDirecTable<A, DS>* rhs_cast(dynamic_cast<const RadDirecTable<A, DS>* >(&rhs));
    // quit if polymorphism not compatible
  if (!rhs_cast) return false;
    // quit true if the directional partition structure is the same 
  return (direcStruct().isEqual(rhs_cast->direcStruct()));
}


 
// RadDirecTable<A,DS>::copy()

template <typename A, typename DS>
void RadDirecTable<A,DS>::copy(const RadDirecDFT<A,DS>& rd_dft) {
  RadDirec::setBandCount(rd_dft.bandCount());
  for (int index=0; index<direcStruct().units(); index++) atIndex(index) = rd_dft.atIndex(index); 
  lazyInvalidate();
}



// RadDirecTable<A,DS>::copyFromMatrixDFT()

template <typename A, typename DS> 
void RadDirecTable<A,DS>::copyFromMatrixDFT(const RD_MatrixDFT& rm) {

  if (!DS::isDFTCapable()) { 
    jlog::es << "RadDirecTable<A,DS>::copyFromMatrixDFT() incompatible directional structure.\n"; 
    internalError(); 
  }

    // construct an empty dft representation with the right number of bands
  RadDirecDFT<A,DS> rd_dft(direcStruct(), rm.bandCount());
    // copy in from the matrices
  rd_dft.copyFromMatrixDFT(rm);
    // copy the dft representation into this
  copy(rd_dft);
}



// RadDirecTable<A,DS>::convertToMatrixDFT()

template <typename A, typename DS> 
void RadDirecTable<A,DS>::copyToMatrixDFT(RD_MatrixDFT& rm) const {

  if (!DS::isDFTCapable()) { 
    jlog::es << "RadDirecTable<A,DS>::copyToMatrixDFT() incompatible directional structure.\n"; 
    internalError(); 
  }

  RadDirecDFT<A,DS>(*this).copyToMatrixDFT(rm);
}



/*! Convienience function which creates the structures for the split data and returns theire pointers in \a f0 and \a f1 */

template <typename A, typename DS>
  void RadDirecTable<A, DS>::splitCreateRotateAboutX(RadDirecTable<A, typename DS::half_type>*& f0, RadDirecTable<A, typename DS::half_type>*& f1) const {
  f0 = duplicateHalfStructureAndDataType();
  f1 = duplicateHalfStructureAndDataType();
  this->splitRotateAboutX(f0,f1);
}



template <typename A, typename DS>
void RadDirecTable<A,DS>::addToData(int rotate_pos, const RadDirecImp<A>& out_data) {

  const RadDirecTable<A,DS>* dt_ptr = dynamic_cast< const RadDirecTable<A,DS>* >(&out_data);
  if (!dt_ptr) { jlog::es << __FUNCTION__ << ":" << __LINE__ << " - incompatible types.\n"; internalError(); }

  if (dt_ptr->bandCount() != RadDirec::bandCount()) {
    jlog::es << __FUNCTION__ << ":" << __LINE__ << " band counts not equal.\n";
    internalError();
  }

  dt.addToData(rotate_pos, dt_ptr->tableData());
} 



//JFactory< RadDirecTable< SBandData, HemiSpherePartVxH > > factory_reg1("rdtable_hemisphere");
//JFactory< RadDirecTable< SBandData, SpherePartVxH > > factory_reg2("rdtable_sphere");



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECTABLE_H




