
// raddirecdirectable.h

#ifndef _JUDE_RAD_RADDIRECDIRECTABLE_H
#define _JUDE_RAD_RADDIRECDIRECTABLE_H

#include "rad/raddirecdirecimp.h"
#include "table/direcdirectable.h"
#include "base/jgenericfactory.h"
#include "rad/radtypes.h"
#include "ds/spherepartvxh.h"
#include "ds/hemispherepartvxh.h"
#include "rad/sbanddata.h"
#include "rad/raddirecdirecdft.h"
#include "rad/raddirecdirecdftsym.h"
#include "rad/rdd_matrixdft.h"

using namespace jude::table;
using namespace jude::rad;
using namespace jude::ds;

namespace jude {
namespace rad {

template <typename A, typename DS>
class RadDirecTable;



template <typename A, typename DS>
class RadDirecDirecTable : public RadDirecDirecImp<A> {

  DirecDirecTable<A, DS> ddt;

  virtual const RadDirecDirecTable<A, DS>* assertCompatible(const RadDirecDirec& rhs, const char* func_name) const {
      // dynamic cast to check direct struct and data type polymorphism
    const RadDirecDirecTable<A, DS>* rhs_cast(dynamic_cast<const RadDirecDirecTable<A, DS>* >(&rhs));
      // message and internal error if polymorphism not compatible
    if (!rhs_cast) { jlog::ls << "Internal error " << func_name << " incompatible polymorphic types.\n"; internalError(); }
      // check that the directional partition structure is the same 
    if (!direcStruct().isEqual(rhs_cast->direcStruct())) {
      jlog::ls << "Internal error " << func_name << " differing directional structures.\n";
      internalError();
    }
    return rhs_cast;
  }
 
public:

  RadDirecDirecTable() { }
  RadDirecDirecTable(const DirecStruct& ds, int bc) : RadDirecDirecImp<A>(bc), ddt(ds) { }
  RadDirecDirecTable(const DirecStruct& ds, int bc, RadDirecDirec::datatype idt, RadDirecDirec::datatype edt) : RadDirecDirecImp<A>(bc, idt,edt), ddt(ds) { }
  RadDirecDirecTable(const RadDirecDirecTable<A,DS>& rhs) : RadDirecDirecImp<A>(rhs), ddt(rhs.ddt) { }
 
  RadDirecDirecTable<A,DS>* duplicate() const { return new RadDirecDirecTable<A,DS>(*this); }

    /*! \brief Creates a empty table with the same directional structure as this. Data type is not copied. */
  RadDirecDirecTable<A,DS>* duplicateStructure() const { return new RadDirecDirecTable<A,DS>(direcStruct(), RadDirecDirec::bandCount()); }

  RadDirecDirecTable<A,DS>* duplicateStructureAndDataType() const { 
    RadDirecDirecTable<A,DS>* rdd = new RadDirecDirecTable<A,DS>(direcStruct(), RadDirecDirec::bandCount()); 
    rdd->copyDataType(*this);
    return rdd;
  }

    /*! \brief Creates a empty table with a hemi directional structure compatible with this. Data type is not copied. */
  RadDirecDirecTable<A, typename DS::half_type>* duplicateHalfStructure() const { 
    return (DS::isFullStatic()) ? new RadDirecDirecTable<A, typename DS::half_type>(direcStruct().half(), RadDirecDirec::bandCount()) : 0; 
  }

  RadDirecTable<A,DS>* createDirec() const;

    // the factory ID needed to reconstruct this object on loading
  virtual const JString factoryID() const { return staticFactoryID(); }
    // safe to assume A and DS are not virtual types
  static const JString staticFactoryID() { return "rddt_"+staticIdOf<A>()+"_"+staticIdOf<DS>(); }

    /*! \brief Returns the directional partition structure. */
  const DS& direcStruct() const { return ddt.direcStruct(); }

    // this is only used by drdz calc in planesolver - fix
  DirecDirecTableBase<A>& tableData() { return ddt; }

  const DirecDirecTableBase<A>& tableData() const { return ddt; }

    /*! \brief Returns the number of cells or 'quads' in the directional partiton structure. */
  int units() const { return ddt.units(); }

    /*! \brief Copy the data in the DFT representation \a rdd_dft into this object. */
  void copy(const RadDirecDirecDFT<A,DS>& rdd_dft);
    /*! \brief Copy the data in the DFT-Symmetric representation \a rdd_dft into this object. */
  void copy(const RadDirecDirecDFTSym<A,DS>& rdd_dft);

    /*! \brief Convert the data in this into matrix-DFT representation and store in \a rdd_mat. */
  void copyToMatrixDFT(RDD_MatrixDFT& rdd_mat) const;
    /*! \brief Convert the data in this into matrix-DFT-Symmetric representation and store in \a rdd_mat. */
  void copyToMatrixDFTSym(RDD_MatrixDFT& rdd_mat) const;

    /*! \brief Try not to use this call, it needs to be depreceated. 
    \todo Depreceate this.*/
  A* dataArray() { return ddt.dataArray(); }; 
    /*! \brief Try not to use this call, it needs to be depreceated.
    \todo Depreceate this.*/
  const A* dataArray() const { return ddt.dataArray(); }; 

    /*! \brief Sets all tabulated entries to \a sd.*/
  void makeAll(const A& sd) { ddt.initAll(sd); }

    /*! \brief Zeros the data in all tabulated entries.*/
  void makeZero() { ddt.initAll(A(RadDirecDirec::bandCount(),0)); }

    /*! \brief Changes the band count to \a bc, filling the band data of each emtry with the current first band value. */
  void expandBandCount(int bc);

  void resample(const BandSpec& targ_bs, const BandSpec& src_bs);
 
  void lazyInvalidate() const { ddt.lazyInvalidate(); }
  
  void multiply(basic_radtype f) { ddt.multiply(f); }
  void divide(basic_radtype f) { ddt.divide(f); }
  
  void multiplyAll(const A& m) { ddt.multiply(m); }
  void divideAll(const A& m) { ddt.divide(m); }
 
  void multiplyQuadrants(double zp_zp, double zp_zm, double zm_zp, double zm_zm) { ddt.multiplyQuadrants(zp_zp, zp_zm, zm_zp, zm_zm); }
 
    /*! Overwrites the current tabulated data at indexed position \a in_index, \a ex_index with \a sd. */
  void setData(int in_index, int ex_index, const A& sd) { ddt.atIndices(in_index, ex_index) = sd; }

    /*! Adds \sd to the current tabulated data at indexed position \a in_index, \a ex_index. */
  void addToData(int in_index, int ex_index, const A& sd) { ddt.atIndices(in_index, ex_index) += sd; }
 
    /*! Returns the function value at the combination of incident and exitant directions \a in_direc and \a ex_direc (both expressed directed outwards). */
  const A at(const Vec3D& in_direc, const Vec3D& ex_direc) const { return ddt.at(in_direc, ex_direc); }

    /*! Returns the function value at the combination of incident and exitant quad indices \a in_index and \a ex_index. */
  const A atIndices(int in_index, int ex_index) const { return ddt.atIndices(in_index, ex_index); }
 
  void copy(const RadDirecDirecTable<A, DS>& r) { ddt.copy(r.ddt); RadDirecDirecImp<A>::copy(r); }
  void add(const RadDirecDirecTable<A, DS>& r) { ddt.add(r.ddt); RadDirecDirecImp<A>::add(r); }
  void addSquared(const RadDirecDirecTable<A, DS>& r) { ddt.addSquared(r.ddt); RadDirecDirecImp<A>::addSquared(r); }
  void subtract(const RadDirecDirecTable<A, DS>& r) { ddt.subtract(r.ddt); RadDirecDirecImp<A>::subtract(r); }
  void subtractSquared(const RadDirecDirecTable<A, DS>& r) { ddt.subtractSquared(r.ddt); RadDirecDirecImp<A>::subtractSquared(r); }
 
  void copy(const RadDirecDirec& rhs) { copy(*assertCompatible(rhs, __FUNCTION__)); }
  void add(const RadDirecDirec& rhs) { add(*assertCompatible(rhs, __FUNCTION__)); }
  void addSquared(const RadDirecDirec& rhs) { addSquared(*assertCompatible(rhs, __FUNCTION__)); }
  void subtract(const RadDirecDirec& rhs) { subtract(*assertCompatible(rhs, __FUNCTION__)); }
  void subtractSquared(const RadDirecDirec& rhs) { subtractSquared(*assertCompatible(rhs, __FUNCTION__)); }

  void squareRoot() { ddt.squareRoot(); RadDirecDirecImp<A>::squareRoot(); }
 
  // note in the following that the units of the data are exDataType() divided by inDataType()
 
    /*! \brief Multiplies all data entries by the solid angle of their corresponding incident quad and modifies the incident data type. */
  void multiplyAllByInSA() { ddt.multiplyAllByInSA(); RadDirecDirec::changeInDataType(RadDirecDirec::SCALAR_IRRAD, RadDirecDirec::MEAN_RAD); }
    /*! \brief Divides all data entries by the solid angle of their corresponding incident quad and modifies the incident data type. */
  void divideAllByInSA() { ddt.divideAllByInSA(); RadDirecDirec::changeInDataType(RadDirecDirec::MEAN_RAD, RadDirecDirec::SCALAR_IRRAD); }
 
    /*! \brief Multiplies all data entries by the solid angle times cos(theta) of their corresponding incident quad and modifies the incident data type. */
  void multiplyAllByInSACT() { ddt.multiplyAllByInSACT(); RadDirecDirec::changeInDataType(RadDirecDirec::PLANAR_IRRAD, RadDirecDirec::MEAN_RAD); }
    /*! \brief Divides all data entries by the solid angle times cos(theta) of their corresponding incident quad and modifies the incident data type. */
  void divideAllByInSACT() { ddt.divideAllByInSACT(); RadDirecDirec::changeInDataType(RadDirecDirec::MEAN_RAD, RadDirecDirec::PLANAR_IRRAD); }
 
    /*! \brief Multiplies all data entries by the solid angle of their corresponding exitant quad and modifies the exitant data type. */
  void multiplyAllByExSA() { ddt.multiplyAllByExSA(); RadDirecDirec::changeExDataType(RadDirecDirec::MEAN_RAD, RadDirecDirec::SCALAR_IRRAD); }
    /*! \brief Divides all data entries by the solid angle of their corresponding exitant quad and modifies the exitant data type. */
  void divideAllByExSA() { ddt.divideAllByExSA(); RadDirecDirec::changeExDataType(RadDirecDirec::SCALAR_IRRAD, RadDirecDirec::MEAN_RAD); }
 
    /*! \brief Multiplies all data entries by the solid angle times cos(theta) of their corresponding exitant quad and modifies the exitant data type. */
  void multiplyAllByExSACT() { ddt.multiplyAllByExSACT(); RadDirecDirec::changeExDataType(RadDirecDirec::MEAN_RAD, RadDirecDirec::PLANAR_IRRAD); }
    /*! \brief Divides all data entries by the solid angle times cos(theta) of their corresponding exitant quad and modifies the exitant data type. */
  void divideAllByExSACT() { ddt.divideAllByExSACT(); RadDirecDirec::changeExDataType(RadDirecDirec::PLANAR_IRRAD, RadDirecDirec::MEAN_RAD); }

  void translateInToOut(const RadDirecTable<A,DS>& rdin, RadDirecTable<A,DS>& rdout) const;
  void translateInToOut(const RadDirecImp<A>& rdin, RadDirecImp<A>& rdout) const;

  void extractOutData(int in_index, int rotate_pos, RadDirecImp<A>& out_data) const;
  void addToOutData(int in_index, int rotate_pos, const RadDirecImp<A>& out_data);
  
  void makeMatrixMultiple(const RadDirecDirecTable<A,DS>& lhs, const RadDirecDirecTable<A,DS>& rhs);

    /*! \brief Returns the sum of all quad-quad entries. */
  const A& sumOfEntries() const { return ddt.sumOfEntries(); } 

    /*! \brief Returns true if this object and \a rhs are of the same polymorphic type and have the same directional partition structure. */
  bool checkCompatible(const RadDirecDirec& rhs) const;
 
    /*! \brief Reads directional partition parameters and table values from a data stream, where source data was created by \a saved_version of the library. */
  bool read(JDataStream& str, uns32 saved_version) { 
      // read band count and data types
    if (!RadDirecDirec::read(str, saved_version)) return false; 
      // read and set up directional structure first
    if (!ddt.readDirecStruct(str, saved_version)) return false;
      // the new way of saving is that the band count is saved once within RadDirec
      // so after getting the directional structure have to initialise all data to the right number of bands
      //! \todo This oculd be done better with a policy class for loading of SBandData that contained the band count.
    if (saved_version >= VERSION_CODE(2, 1, 0)) { makeAll(A(RadDirecDirec::bandCount(),0)); }
      // read the data
    if (!ddt.readData(str, saved_version)) return false;
      // In the old way of saving overall band count is not saved but each SbandData saved the band count
    if (saved_version < VERSION_CODE(2, 1, 0)) RadDirecDirec::setBandCount(atIndices(0,0).bandCount());
    return true;
  }
  
  bool write(JDataStream& str) const { 
    if (!RadDirecDirec::write(str)) return false; 
    return ddt.write(str); 
  }
  
  bool read(JTextStream& str) { return false; }
  bool write(JTextStream& str) const { return false; }
};



// RadDirecDirecTable<A,DS>::expandBandCount()

template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::expandBandCount(int bc) {
  A* ptr = ddt.dataArray();
  const A* ptr_end = ddt.dataArray() + ddt.totalUnits();
  for (; ptr!=ptr_end; ptr++) (*ptr) = A(bc,(*ptr)[0]);
  RadDirecDirec::setBandCount(bc);
}



// RadDirecDirecTable<A,DS>::resample()

template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::resample(const BandSpec& targ_bs, const BandSpec& src_bs) {
  A* ptr = ddt.dataArray();
  const A* ptr_end = ddt.dataArray() + ddt.totalUnits();
  for (; ptr!=ptr_end; ptr++) (*ptr) = targ_bs.resampleToThis(*ptr, src_bs);
  RadDirecDirec::setBandCount(targ_bs.bandCount());
}



// RadDirecDirecTable<A,DS>::checkCompatible()

template <typename A, typename DS>
bool RadDirecDirecTable<A,DS>::checkCompatible(const RadDirecDirec& rhs) const {
    // dynamic cast to check direct struct and data type polymorphism
  const RadDirecDirecTable<A, DS>* rhs_cast(dynamic_cast<const RadDirecDirecTable<A, DS>* >(&rhs));
    // quit if polymorphism not compatible
  if (!rhs_cast) return false;
    // quit true if the directional partition structure is the same 
  return (direcStruct().isEqual(rhs_cast->direcStruct()));
}


} } // namespace jude::rad



#include "raddirectable.h"

namespace jude {
namespace rad {



template <typename A, typename DS>
RadDirecTable<A,DS>* RadDirecDirecTable<A,DS>::createDirec() const { 
  return new RadDirecTable<A,DS>(direcStruct(), RadDirecDirec::bandCount()); 
}



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::translateInToOut(const RadDirecTable<A,DS>& rdin, RadDirecTable<A,DS>& rdout) const {
  rdout.band_count = rdin.band_count;
  ddt.translateInToOut(rdin.tableData(), rdout.tableData());
}



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::translateInToOut(const RadDirecImp<A>& rdin, RadDirecImp<A>& rdout) const {

  const RadDirecTable<A,DS>* ptr_in = dynamic_cast< const RadDirecTable<A,DS>* >(&rdin);
  RadDirecTable<A,DS>* ptr_out = dynamic_cast< RadDirecTable<A,DS>* >(&rdout);
  if (ptr_in && ptr_out) return translateInToOut(*ptr_in, *ptr_out);
  jlog::es << "RadDirecDirecTable:: translateInToOut() - unsupported input type(s).\n";
  internalError();
}



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::extractOutData(int in_index, int rotate_pos, RadDirecImp<A>& out_data) const {

  RadDirecTable<A,DS>* dt_ptr = dynamic_cast< RadDirecTable<A,DS>* >(&out_data);
  if (!dt_ptr) { jlog::es << __FUNCTION__ << ":" << __LINE__ << " - incompatible types.\n"; internalError(); }

  dt_ptr->setBandCount(RadDirecDirec::bandCount());

  ddt.extractOutData(in_index, rotate_pos, dt_ptr->tableData());
}



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::addToOutData(int in_index, int rotate_pos, const RadDirecImp<A>& out_data) {

  const RadDirecTable<A,DS>* dt_ptr = dynamic_cast< const RadDirecTable<A,DS>* >(&out_data);
  if (!dt_ptr) { jlog::es << __FUNCTION__ << ":" << __LINE__ << " - incompatible types.\n"; internalError(); }

  if (dt_ptr->bandCount() != RadDirecDirec::bandCount()) {
    jlog::es << __FUNCTION__ << ":" << __LINE__ << " band counts not equal.\n";
    internalError();
  }
  ddt.addToOutData(in_index, rotate_pos, dt_ptr->tableData());
} 



// RadDirecDirecTable<A,DS>::makeMatrixMultiple()

//! Like matrix multiplcation of (lhs) * (rhs).
//! Result is a combined reflectance of all routes from (lhs) in_index -> (rhs) out_index
//! Each value is the sum over all (lhs) out_indices -> (rhs) in_indices.

template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::makeMatrixMultiple(const RadDirecDirecTable<A,DS>& lhs, const RadDirecDirecTable<A,DS>& rhs) {

  const DS& ds = direcStruct();

    // go through the indices of the result
  for (int in_index=0; in_index<ds.units(); in_index++) {
    for (int out_index=0; out_index<ds.units(); out_index++) {

      A v(RadDirecDirec::bandCount(), 0);

        // the index to sum over (lhs out_index and rhs in_index)
      for (int sum_index=0; sum_index<ds.units(); sum_index++) {
	v += lhs.atIndices(in_index, sum_index) * rhs.atIndices(sum_index, out_index);
      }

        // set the value in the result
      setData(in_index, out_index, v);
    }
  }
}



// RadDirecDirecTable<A,DS>::copy()

template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::copy(const RadDirecDirecDFT<A,DS>& rdd_dft) {

  RadDirecDirec::setBandCount(rdd_dft.bandCount());

  for (int in_index=0; in_index<direcStruct().units(); in_index++) {
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {
      setData(in_index, out_index, rdd_dft.atIndices(in_index, out_index)); 
    }
  }

  lazyInvalidate();
}



// RadDirecDirecTable<A,DS>::copy()

template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::copy(const RadDirecDirecDFTSym<A,DS>& rdd_dft) {

  RadDirecDirec::setBandCount(rdd_dft.bandCount());

  for (int in_index=0; in_index<direcStruct().units(); in_index++) {
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {
      atIndices(in_index, out_index) = rdd_dft.atIndices(in_index, out_index); 
    }
  }

  lazyInvalidate();
}



// RadDirecTable<A,DS>::copyFromMatrixDFT()

//template <typename A, typename DS> 
//void RadDirecDirecTable<A,DS>::copyFromMatrixDFTSym(const RDD_MatrixDFT& rdd_mat) {

//if (!DS::isDFTCapable()) { 
//   jlog::es << "RadDirecDirecTable<A,DS>::copyFromMatrixDFTSym() incompatible directional structure.\n"; 
//   internalError(); 
// }

// RadDirecDirecDFTSym<A,DS>(*this).copyFromMatrixDFT(rdd_mat);
//}



// RadDirecDirecTable<A,DS>::convertToMatrixDFT()

template <typename A, typename DS> 
void RadDirecDirecTable<A,DS>::copyToMatrixDFT(RDD_MatrixDFT& rdd_mat) const {

  if (!DS::isDFTCapable()) { 
    jlog::es << "RadDirecDirecTable<A,DS>::copyToMatrixDFT() incompatible directional structure.\n"; 
    internalError(); 
  }

  RadDirecDirecDFT<A,DS>(*this).copyToMatrixDFT(rdd_mat);
}



// RadDirecDirecTable<A,DS>::convertToMatrixDFT()

template <typename A, typename DS> 
void RadDirecDirecTable<A,DS>::copyToMatrixDFTSym(RDD_MatrixDFT& rdd_mat) const {

  if (!DS::isDFTCapable()) { 
    jlog::es << "RadDirecDirecTable<A,DS>::copyToMatrixDFTSym() incompatible directional structure.\n"; 
    internalError(); 
  }

  RadDirecDirecDFTSym<A,DS>(*this).copyToMatrixDFT(rdd_mat);
}



} } // namespace jude::rad



#endif // _JUDE_RAD_RADDIRECDIRECTABLE_H

