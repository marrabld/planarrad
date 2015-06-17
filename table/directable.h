
// directable.h

#ifndef _JUDE_TABLE_DIRECTABLE_H
#define _JUDE_TABLE_DIRECTABLE_H

#include "table/directablebase.h"
#include "table/direcdirectablebase.h"
#include "S3/vec3d.h"
#include "base/jfile.h"
#include "ds/dsquadipol.h"
#include "ds/direcstruct.h"
#include "S3/basis3d.h"
#include "base/jerror.h"
#include "base/jlog.h"

using namespace jude::ds;
using namespace jude::base;

namespace jude {
namespace table {



/*! \brief Stores objects of type A in a table orgainsed according to the directional parition structure DS. */

template <typename A, typename DS> 
class DirecTable : public DirecTableBase<A> {

  DS ds;
  A* data; 

  mutable bool lazy_valid;

  mutable A entry_sum;
  mutable A entry_times_sa_sum;
  mutable A entry_times_sa_abs_cos_theta_sum;
  mutable A entry_times_abs_cos_theta_sum;

  void allocate(int n) {
    delete[] data;
    data = new A[n]; 
  }

protected:

  inline void assertIdenticalDS(const DirecStruct& check_ds, const char* func_name) {
    if (ds.isEqual(check_ds)) return;
    jlog::es << "Internal error " << func_name << " incompatible directional structure\n"; 
    internalError(); 
  }
 
  void lazyCalc() const;
 
public:

  DirecTable<A,DS>() : data(0) { }

  DirecTable<A,DS>(const DS& ds) : 
    ds(ds), 
    data(new A[ds.units()]), 
    lazy_valid(false) { }

  DirecTable<A,DS>(const DS& ds, JDataStream& str) : 
    ds(ds), 
    data(new A[ds.units()]), 
    lazy_valid(false) { read(str); }

  DirecTable<A,DS>(const DS& ds, JTextStream& str) : 
    ds(ds), 
    data(new A[ds.units()]), 
    lazy_valid(false) { read(str); }

  DirecTable<A,DS>(const DirecTable<A,DS>& dt) :
    ds(dt.ds), 
    data(new A[ds.units()]),   
    lazy_valid(false) { copy(dt); }

  DirecTable<A,DS>(const DirecStruct& ids) :
    ds(ids), 
    data(new A[ds.units()]),   
    lazy_valid(false) { }

  /*
  DirecTable(DumpStreamIn& str) :
    ds(str),
    data(new A[ds.units()]) { 
    read(str);  
    str >> lazy_valid; 
    str >> entry_sum;
    str >> entry_times_sa_sum;
    str >> entry_times_sa_abs_cos_theta_sum;
  }
  */

  ~DirecTable() { delete[] data; }

  /*
  void dumpWrite(DumpStreamOut& str) const { 
    ds.dumpWrite(str); 
    write(str);
    str << lazy_valid;
    str << entry_sum;
    str << entry_times_sa_sum;
    str << entry_times_sa_abs_cos_theta_sum;
  }
  */
 
  A* dataArray() { return data; }; 
  const A* dataArray() const { return data; }; 

  const DS& direcStruct() const { return ds; }

  template <typename F>
  DirecTableBase<F>* duplicateStructure() const { return new DirecTable<F,DS>(ds); }

    // this requires (type A) = (type A) to be defined 
  void copy(const DirecTable<A,DS>& dt) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    A* to(data);
    const A* from(dt.data);
    const A* const end(data + ds.units());
    for (;to!=end;to++,from++) *to = *from;
    lazy_valid = false;
  }

    // this requires (type A) = (type A) to be defined 
  void copyInverted(const DirecTable<A,DS>& dt) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    if (!ds.isFull()) { jlog::ls << __FUNCTION__ << " not-full directional structure"; internalError(); }
    for (int i=0;i<ds.units();i++) data[i] = dt.data[ds.negatedIndex(i)];
    lazy_valid = false;
  }

    // this requires (type A) = (type A) to be defined 
  void initAll(const A& a) {
    A* ptr(data);
    const A* const end(data + ds.units());
    while (ptr!=end) (*ptr++) = a;
    lazy_valid = false;
  }

     /*! \brief Sets all tabulated entries to \a a where directional z-value is greater than zero.
     Requires (type A) = (type A) to be defined */
  void initAllZPlus(const A& a) {
    for (int i=0;i<ds.units();i++) if (ds.isZPlus(i)) data[i] = a;
    lazy_valid = false;
  }

     /*! \brief Sets all tabulated entries to \a a where directional z-value is less than zero.
     Requires (type A) = (type A) to be defined */
  void initAllZMinus(const A& a) {
    for (int i=0;i<ds.units();i++) if (ds.isZMinus(i)) data[i] = a;
    lazy_valid = false;
  }

    // this requires (type A) += (type F) to be defined 
  template <typename F>
  void add(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.units());
    while (ptr!=end) (*ptr++) += f;
    lazy_valid = false;
  }

    // this requires (type A) -= (type F) to be defined 
  template <typename F>
  void subtract(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.units());
    while (ptr!=end) (*ptr++) -= f;
    lazy_valid = false;
  }

    // this requires (type A) *= (type F) to be defined
  template <typename F>
  void multiply(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.units());
    while (ptr!=end) (*ptr++) *= f;
    lazy_valid = false;
  }

    // this requires (type A) /= (type F) to be defined
  template <typename F>
  void divide(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.units());
    while (ptr!=end) (*ptr++) /= f;
    lazy_valid = false;
  }

    // this requires (type A) += (type F) to be defined
  template <typename F>
  void add(const DirecTable<F,DS>& dt) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    A* to(data);
    const F* from(dt.dataArray());
    const A* const end = to + ds.units();
    while (to!=end) (*to++) += (*from++);
    lazy_valid = false;
  }

    // this requires (type A) -= (type F) to be defined
  template <typename F>
  void subtract(const DirecTable<F,DS>& dt) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    A* to(data);
    const F* from(dt.dataArray());
    const A* const end = to + ds.units();
    while (to!=end) (*to++) -= (*from++);
    lazy_valid = false;
  }

   // add on another directable of data that is non aligned or of a differnt directional structure
   // this requires (type A) += (type F) to be defined 
   // (bas) is basis of (dt) relative to this
  template <typename F, typename DS2>
  void addNonAligned(const DirecTable<F,DS2>& dt, const Basis3D& bas) {
    for (int i=0;i<ds.units();i++) {
      Vec3D dv(bas.intoBasis(ds.indexToDirec(i)));
      //Vec3D dv(ds.indexToDirec(i));
      data[i] += dt.at(dv);
    }
    lazy_valid = false;
  }

    // this requires (type A) /= (type F) to be defined
  template <typename F>
  void divide(const DirecTable<F,DS>& dt) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    A* to(data);
    const F* from(dt.dataArray());
    const A* const end = to + ds.units();
    while (to!=end) (*to++) /= (*from++);
    lazy_valid = false;
  }

    // this requires (type A) += (type F) and (type F) * (type M) to be defined
  template <typename F, typename M>
  void addMultiple(const DirecTable<F,DS>& dt, const M& m) {
    assertIdenticalDS(dt.direcStruct(), __FUNCTION__);
    A* ptr0(data);
    A* ptr1(dt.data);
    const A* const end(data + ds.units());
    while (ptr0!=end) (*ptr0++) += (*ptr1++) * m;
    lazy_valid = false;
  }

    // this requires (type A) /= (double) to be defined
  void divideAllBySA() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) /= ds.solidAngle(i);
    lazy_valid = false;
  }

    // this requires (type A) *= (double) to be defined
  void multiplyAllBySA() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) *= ds.solidAngle(i);
    lazy_valid = false;
  }

    // this requires (type A) *= (double) to be defined
  void multiplyAllBySACT() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) *= ds.solidAngleAbsCosTheta(i);
    lazy_valid = false;
  }

    // this requires (type A) /= (double) to be defined
  void divideAllBySACT() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) /= ds.solidAngleAbsCosTheta(i);
    lazy_valid = false;
  }

    // this requires (type A) *= (double) to be defined
  void multiplyAllByCT() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) *= fabs(ds.meanCosTheta(i));
    lazy_valid = false;
  }

    // this requires (type A) /= (double) to be defined
  void divideAllByCT() {
    A* ptr(data);
    const A* const end(data + ds.units());
    for (int i=0;ptr!=end;i++,ptr++) (*ptr) /= fabs(ds.meanCosTheta(i));
    lazy_valid = false;
  }

    // average the values int the table to make fully symmetric with respect 
    // to the symmetries coded in the directional structure
  void makeFullSymmetry();

    // mark any internal lazy calculations as invalid
  void lazyInvalidate() const {
    lazy_valid = false;
  }

    // lazy calculate and return the sum of the entries
    // note requires explicit invalidation by lazyInvalidate()
    // this requires (type A) += (type A) to be defined
  const A& sumOfEntries() const {
    if (!lazy_valid) lazyCalc();
    return entry_sum;
  }

    // lazy calculate and return the solid angle wieghted mean of the entries
    // note requires explicit invalidation by lazyInvalidate()
    // this requires (type A) += (type A), (type A) * (double) and (type A) /= (double) are defined
  const A& sumBySolidAngle() const {
    if (!lazy_valid) lazyCalc();
    return entry_times_sa_sum;
  }
 
    // the sum of entries multiplied by their solid angle and cos theta to the z direction (surface normal)
    // note requires explicit invalidation by lazyInvalidate()
    // this requires that (type A) += (type A) are (type A) * (double) are defined
  const A& sumBySolidAngleAbsCosTheta() const {
    if (!lazy_valid) lazyCalc();
    return entry_times_sa_abs_cos_theta_sum;
  }

    // the sum of entries multiplied by solid angle mean cos theta to the z direction
    // note requires explicit invalidation by lazyInvalidate()
    // this requires that (type A) += (type A) are (type A) * (double) are defined
  const A& sumByAbsCosTheta() const {
    if (!lazy_valid) lazyCalc();
    return entry_times_abs_cos_theta_sum;
  }

  void sumBySolidAngle(A& zp, A& zm) const;
  void sumBySolidAngleAbsCosTheta(A& zp, A& zm) const;

  // probably depreceated
  //void applyToAll(void(*func)(A&)) {
  //  A* ptr(data);
  //  const A* const end(data + ds.units());
  //  while (ptr!=end) func(*(ptr++));
  //}

  double proportionalArea(int i) const { return ds.proportionalArea(i); }
  double solidAngle(int i) const { return ds.solidAngle(i); }
  double solidAngleAbsCosTheta(int i) const { return ds.solidAngleAbsCosTheta(i); }

  int units() const { return ds.units(); }

  int direcToIndex(const Vec3D& direc_out) const { return ds.direcToIndex(direc_out); }
  Vec3D indexToDirec(int i) const { return ds.indexToDirec(i); }

  //double thetaFromIndex(int i) const { return ds.thetaFromIndex(i); }
  //int windowToIndex(int v, int h) const { return ds.windowToIndex(v,h); }

  A& at(const Vec3D& out_direc) { return data[ds.direcToIndex(out_direc)]; }
  const A& at(const Vec3D& out_direc) const { return data[ds.direcToIndex(out_direc)]; }

  const A interpolated(const Vec3D& out_direc) const {
    DsQuadIpol dqi;
    ds.direcToInterpolated(out_direc, dqi);
    return dqi.buildData(data);
  }

  A& atIndex(int i) { return data[i]; }
  const A& atIndex(int i) const { return data[i]; }

  //A& atAdaptiveWindow(int x, int y) { return data[ds.indexToAdapBaseadaptiveTableToIndex(0,v,h)]; }
  //const A& atAdaptiveWindow(int x, int y) const { return data[ds.adaptiveTableToIndex(0,v,h)]; }

  const A divSolidAngle(const Vec3D& out_direc) const {
    int i = ds.direcToIndex(out_direc);
    return data[i] / ds.solidAngle(i); 
  }

  void addToData(int rotate_pos, const DirecTable<A,DS>& dt_out);

    // return minimum value in table
    // this requires (type A) < (type A) to be defined 
  const A min() const {
    const A* curr_min(data);
    A* ptr(data);
    const A* const end(data + ds.units());
    while ((++ptr)!=end) if (*ptr < *curr_min) curr_min = ptr;
    return *curr_min;
  }

    // return maximum value in table
    // this requires (type A) > (type A) to be defined 
  const A max() const {
    const A* curr_max(data);
    A* ptr(data);
    const A* const end(data + ds.units());
    while ((++ptr)!=end) if (*ptr > *curr_max) curr_max = ptr;
    return *curr_max;
  }

    // requires operator>> from a JDataStream to type A
  //void read(JDataStream& str) {
  //  A* to(data);
  //  const A* const end(to + ds.units());
  //  for (;to!=end;to++) str >> *to; 
  //  lazy_valid = false;
  // }
 
    // requires operator<< to a JDataStream from type A
  //void write(JDataStream& str) const {
  //  const A* from(data);
  //  const A* const end(from + ds.units());
  //  for (;from!=end;from++) str << *from;
  // }
 
    // requires operator>> from a JTextStream to type A
  //void read(JTextStream& str) {
  //  A* to(data);
  //  const A* const end(to + ds.units());
  //  for (;to!=end;to++) str >> *to; 
  //  lazy_valid = false;
  // }


    // requires operator<< to a JTextStream from type A
  void write(JTextStream& str) const {
    const A* from(data);
    const A* const end(from + ds.units());
    for (;from!=end;from++) str << *from;
  }
 
  bool read(JDataStream& str, uns32 saved_version);
  bool readDirecStruct(JDataStream& str, uns32 saved_version);
  bool readData(JDataStream& str, uns32 saved_version);
  bool write(JDataStream& str) const;
 
  void display() const {
    const A* from(data);
    const A* const end(from + ds.units());
    for (int i=0;from!=end;from++,i++) {
      printf("%d: ",i);
      //GlobalFuncs::display(*from);
      printf("\n");
    }
  }
};



// DirecTable::makeFullSymmetry()

template <typename A, typename DS> 
void DirecTable<A,DS>::makeFullSymmetry() {

    // if no symmetry data then quit
  if (ds.symmetricalTransNum()==0) return;

    // create a copy of the table
  const DirecTable<A,DS> dt(*this);

    // loop through all indices
  for (int i=0;i<ds.units();i++) {

      // translated index values for different orientations
    const int* index_list = ds.symmetricalIndices(i);  

    A v(0);
    for (int j=0;j<ds.symmetricalTransNum();j++) v += dt.atIndex(index_list[j]);

    v /= ds.symmetricalTransNum();

    data[i] = v;
  }
}



// DirecTable::lazyCalc() [private]

template <typename A, typename DS> 
void DirecTable<A,DS>::lazyCalc() const {

  entry_sum = A(0);
  entry_times_sa_sum = A(0);
  entry_times_sa_abs_cos_theta_sum = A(0);
  entry_times_abs_cos_theta_sum = A(0);
  
  A* ptr(data);
  const A* const end(data + ds.units());
  for (int i=0;ptr!=end;i++,ptr++) {
      // straight sum of entries
    entry_sum += (*ptr);
      // mean of entries weighted by solid angle
    entry_times_sa_sum += (*ptr) * ds.solidAngle(i);
      // sum of entries weighted by solid angle abs cos theta
    entry_times_sa_abs_cos_theta_sum += (*ptr) * ds.solidAngleAbsCosTheta(i);
      // sum of entries weighted by abs cos theta
    entry_times_abs_cos_theta_sum += (*ptr) * ds.meanCosTheta(i);
  }
 
  lazy_valid = true;
}



// DirecTable::sumBySolidAngle()
// this version sums upward (z>0) and downward (z<0) seperately i.e. Eu and Ed

template <typename A, typename DS> 
void DirecTable<A,DS>::sumBySolidAngle(A& zp, A& zm) const {

  zp = A(0);
  zm = A(0);

  A* ptr(data);
  const A* const end(data + ds.units());
  for (int i=0;ptr!=end;i++,ptr++) {
      // sum of entries weighted by solid angle abs cos theta upwards and downwards
    if (ds.isZPlus(i)) zp += (*ptr) * ds.solidAngle(i);
    else zm += (*ptr) * ds.solidAngle(i);
  }
}



// DirecTable::sumBySolidAngleAbsCosTheta()
// this version sums upward (z>0) and downward (z<0) seperately i.e. Eu and Ed

template <typename A, typename DS> 
void DirecTable<A,DS>::sumBySolidAngleAbsCosTheta(A& zp, A& zm) const {

  zp = A(0);
  zm = A(0);

  A* ptr(data);
  const A* const end(data + ds.units());
  for (int i=0;ptr!=end;i++,ptr++) {
      // sum of entries weighted by solid angle abs cos theta upwards and downwards
    if (ds.isZPlus(i)) zp += (*ptr) * ds.solidAngleAbsCosTheta(i);
    else zm += (*ptr) * ds.solidAngleAbsCosTheta(i);
  }
}



template <typename A, typename DS> 
void DirecTable<A,DS>::addToData(int rotate_pos, const DirecTable<A,DS>& dt_out) {

    // loop through indices of output structure
  for (int index=0; index<ds.units(); index++) {
      // the out-index on the function table
    int dt_index = ds.azimuthallyRotatedIndex(rotate_pos, index);
      // set data in the output table
    atIndex(dt_index) += dt_out.atIndex(index);
  }
} 



/*! Reads in a data from data stream into a constructed DirecTable. */

template <typename A, typename DS>
bool DirecTable<A,DS>::read(JDataStream& str, uns32 saved_version) {
    // read in the directional partition
  ds.read(str);
    // ensure memory is allocated
  allocate(ds.units());
    // read in the data
  A* to(data);
  const A* const end(to + ds.units());
  for (;to!=end;to++) if (!readFromDataStream(*to, str, saved_version)) return false; 
    // calculated values are no longer valid
  lazy_valid = false;
  return true;
}



template <typename A, typename DS>
bool DirecTable<A,DS>::readDirecStruct(JDataStream& str, uns32 saved_version) {
    // read in the directional partition
  ds.read(str);
    // ensure memory is allocated
  allocate(ds.units());
  return true;
}



template <typename A, typename DS>
bool DirecTable<A,DS>::readData(JDataStream& str, uns32 saved_version) {
    // read in the data
  A* to(data);
  const A* const end(to + ds.units());
  for (;to!=end;to++) if (!readFromDataStream(*to, str, saved_version)) return false; 
    // calculated values are no longer valid
  lazy_valid = false;
  return true;
}



/*! Writes data to a data stream. */

template <typename A, typename DS>
bool DirecTable<A,DS>::write(JDataStream& str) const {
    // write the directional partiton
  ds.write(str);
    // write the data
  const A* from(data);
  const A* const end(from + ds.units());
  for (;from!=end;from++) if (!writeToDataStream(*from, str)) return false;
  return true;
}



} } // namespace jude::table

#endif // _JUDE_TABLE_DIRECTABLE_H

