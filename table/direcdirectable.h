
// direcdirectable.h

#ifndef _JUDE_TABLE_DIRECDIRECTABLE_H
#define _JUDE_TABLE_DIRECDIRECTABLE_H

#include "table/direcdirectablebase.h"
#include "S3/vec3d.h"
#include "base/jerror.h"
#include "base/jlog.h"
#include "table/directable.h"
//#include "direcadap.h"

using namespace jude::ds;
using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace table {



/*! \brief Stores objects of type A in a 2D table orgainsed according to direction to direction based on parition structure DS. */

template <typename A, typename DS> 
class DirecDirecTable : public DirecDirecTableBase<A> {

  DS ds;
  A* data; 

  mutable bool lazy_valid;
  mutable A entry_sum;

  void allocate(int n) {
    delete[] data;
    data = new A[n]; 
  }

protected: 

  inline void checkIdenticalDS(const DirecStruct& check_ds, const char* func_name) {
    if (ds.isEqual(check_ds)) return;
    jlog::es << "Internal error " << func_name << " incompatible directional structure\n"; 
    internalError(); 
  }
 
  void lazyCalc() const;

public:

  typedef DS ds_type;

  DirecDirecTable<A,DS>() : data(0) { }

  DirecDirecTable<A,DS>(const DS& ds) : 
    ds(ds), 
    data(new A[ds.unitsSquared()]), 
    lazy_valid(false) { }

  DirecDirecTable<A,DS>(const DS& ds, JDataStream& str) : 
    ds(ds), 
    data(new A[ds.unitsSquared()]),
    lazy_valid(false) { read(str); }

  DirecDirecTable<A,DS>(const DS& ds, JTextStream& str) : 
    ds(ds), 
    data(new A[ds.unitsSquared()]),
    lazy_valid(false) { read(str); }

  DirecDirecTable<A,DS>(const DirecDirecTable<A,DS>& dt) :
    ds(dt.ds), 
    data(new A[ds.unitsSquared()]),
    lazy_valid(false) { copy(dt); }

  //  DirecDirecTable<A,DS>(DumpStreamIn& str) : 
  //  ds(str),
  //  data(new A[ds.unitsSquared()]) {
  //  read(str);
  //  str >> lazy_valid;
  //  str >> entry_sum;
  // }

  ~DirecDirecTable() { delete[] data; }

  //void dumpWrite(DumpStreamOut& str) const { 
  //  ds.dumpWrite(str); 
  //  write(str);
  // str << lazy_valid;
  //  str << entry_sum;
  // }

  A* dataArray() { return data; }; 
  const A* dataArray() const { return data; }; 

  int baseUnits() const { return ds.units(); }
  int totalUnits() const { return ds.unitsSquared(); }

  const DS& direcStruct() const { return ds; }

  template <typename F>
  DirecDirecTableBase<F>* duplicateStructure() const { return new DirecDirecTable<F,DS>(ds); }

    // this requires (type A) = (type A) to be defined 
  void copy(const DirecDirecTable<A,DS>& ddt) {
    if (!ds.isEqual(ddt.direcStruct())) { jlog::es << "Internal error " << __FUNCTION__ << " incompatible directional structure"; internalError(); }
    A* to(data);
    const A* from(ddt.data);
    const A* const end(data + ds.unitsSquared());
    for (;to!=end;to++,from++) *to = *from;
    entry_sum = ddt.entry_sum;
    lazy_valid = ddt.lazy_valid;
  }

    // this requires (type A) = (type A) to be defined 
  void copyInvertedIn(const DirecDirecTable<A,DS>& ddt) {
    if (!ds.isEqual(ddt.direcStruct())) { jlog::es << "Internal error " << __FUNCTION__ << " incompatible directional structure"; internalError(); }
    if (!ds.isFull()) { jlog::es << "Internal error " << __FUNCTION__ << " non-full directional structure"; internalError(); }

    for (int in_index=0; in_index<ds.units(); in_index++) {
      for (int out_index=0; out_index<ds.units(); out_index++) {
	atIndices(ds.negatedIndex(in_index), out_index) = atIndices(in_index, out_index);
      }
    }
  }

    // this requires (type A) = (type A) to be defined 
  void initAll(const A& a) {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) (*ptr++) = a;
    lazy_valid = false;
  }

    // this requires (type A) = (type A) to be defined
    // sets all values in the "upper quadrant" i.e. z_in>0 and z_out>0
  void initAllZPlusZPlus(const A& a) {
    for (int in_index=0; in_index<ds.units(); in_index++) {
      for (int out_index=0; out_index<ds.units(); out_index++) {
	if (ds.indexToDirec(in_index).z()>0 && ds.indexToDirec(out_index).z()>0) atIndices(in_index,out_index) = a;
      }
    }
    lazy_valid = false;
  }

    // this requires (type A) *= (type F) to be defined
  template <typename F>
  void multiplyQuadrants(const F& zp_zp, const F& zp_zm, const F& zm_zp, const F& zm_zm) {
    for (int in_index=0; in_index<ds.units(); in_index++) {
      for (int out_index=0; out_index<ds.units(); out_index++) {
	if (ds.isZPlus(in_index)) {
	  if (ds.isZPlus(out_index)) atIndices(in_index, out_index) *= zp_zp;
	  else atIndices(in_index, out_index) *= zp_zm;
	} else {
	  if (ds.isZPlus(out_index)) atIndices(in_index, out_index) *= zm_zp;
	  else atIndices(in_index, out_index) *= zm_zm;
	}
      }
    }
    lazy_valid = false;
  }

    // this requires (type A) += (type F) to be defined
  template <typename F>
  void add(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) (*ptr++) += f;
    lazy_valid = false;
  }

    // this requires (type A) *= (type F) to be defined
  template <typename F>
  void multiply(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) (*ptr++) *= f;
    lazy_valid = false;
  }

    // this requires (type A) /= (type F) to be defined
  template <typename F>
  void divide(const F& f) {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) (*ptr++) /= f;
    lazy_valid = false;
  }

    // this requires (type A) += (type F) to be defined
  template <typename F>
  void add(const DirecDirecTable<F,DS>& ddt) {
    A* ptr0(data);
    A* ptr1(ddt.data);
    const A* const end(data + ds.unitsSquared());
    while (ptr0!=end) (*ptr0++) += (*ptr1++);
    lazy_valid = false;
  }

    // this requires (type A) += (type F) and (type F) * (type F) to be defined
  template <typename F>
  void addSquared(const DirecDirecTable<F,DS>& ddt) {
    A* ptr0(data);
    A* ptr1(ddt.data);
    const A* const end(data + ds.unitsSquared());
    while (ptr0!=end) { (*ptr0++) += (*ptr1) * (*ptr1); ptr1++; }
    lazy_valid = false;
  }

    // this requires (type A) -= (type F) and (type F) * (type F) to be defined
  template <typename F>
  void subtractSquared(const DirecDirecTable<F,DS>& ddt) {
    A* ptr0(data);
    A* ptr1(ddt.data);
    const A* const end(data + ds.unitsSquared());
    while (ptr0!=end) { (*ptr0++) -= (*ptr1) * (*ptr1); ptr1++; }
    lazy_valid = false;
  }

    // this requires (type A) -= (type F) to be defined
  template <typename F>
  void subtract(const DirecDirecTable<F,DS>& ddt) {
    A* ptr0(data);
    A* ptr1(ddt.data);
    const A* const end(data + ds.unitsSquared());
    while (ptr0!=end) (*ptr0++) -= (*ptr1++);
    lazy_valid = false;
  }

   // this requires sqrt(A) to be defined
  void squareRoot() {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) { (*ptr) = sqrt(*ptr); ptr++; }
    lazy_valid = false;
  }

    // multiply each entry by its corresponding in-direction solid angle
    // this requires (type A) *= (double) to be defined
  void multiplyAllByInSA() {
    for (int in_index=0;in_index<ds.units();in_index++) {
      double sa = ds.solidAngle(in_index);
      A* ptr(data+in_index*ds.units());
      const A* const end(ptr + ds.units());
      for (;ptr!=end;ptr++) (*ptr) *= sa;
    }
    lazy_valid = false;
  }

    // multiply each entry by its corresponding exitant direction solid angle
    // this requires (type A) *= (double) to be defined
  void multiplyAllByExSA() {
    for (int out_index=0;out_index<ds.units();out_index++) {
      double sa = ds.solidAngle(out_index);
      A* ptr(data+out_index);
      const A* const end(ptr + ds.unitsSquared());
      for (;ptr!=end;ptr+=ds.units()) (*ptr) *= sa;
    }
    lazy_valid = false;
  }

    // multiply each entry by its corresponding in direction solid angle times cos(theta)
    // this requires (type A) *= (double) to be defined
  void multiplyAllByInSACT() {
    for (int in_index=0;in_index<ds.units();in_index++) {
      double sact = ds.solidAngleAbsCosTheta(in_index);
      A* ptr(data+in_index*ds.units());
      const A* const end(ptr + ds.units());
      for (;ptr!=end;ptr++) (*ptr) *= sact;
    }
    lazy_valid = false;
  }

    // multiply each entry by its corresponding exitant direction solid angle times cos(theta)
    // this requires (type A) *= (double) to be defined
  void multiplyAllByExSACT() {
    for (int out_index=0;out_index<ds.units();out_index++) {
      double sact = ds.solidAngleAbsCosTheta(out_index);
      A* ptr(data+out_index);
      const A* const end(ptr + ds.unitsSquared());
      for (;ptr!=end;ptr+=ds.units()) (*ptr) *= sact;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding in-direction solid angle
    // this requires (type A) *= (double) to be defined
  void divideAllByInSA() {
    for (int in_index=0;in_index<ds.units();in_index++) {
      double sa_recip = 1.0 / ds.solidAngle(in_index);
      A* ptr(data+in_index*ds.units());
      const A* const end(ptr + ds.units());
      for (;ptr!=end;ptr++) (*ptr) *= sa_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding exitant direction solid angle
    // this requires (type A) *= (double) to be defined
  void divideAllByExSA() {
    for (int out_index=0;out_index<ds.units();out_index++) {
      double sa_recip = 1.0 / ds.solidAngle(out_index);
      A* ptr(data+out_index);
      const A* const end(ptr + ds.unitsSquared());
      for (;ptr!=end;ptr+=ds.units()) (*ptr) *= sa_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding in direction solid angle times cos(theta)
    // this requires (type A) *= (double) to be defined
  void divideAllByInSACT() {
    for (int in_index=0;in_index<ds.units();in_index++) {
      double sact_recip = 1.0 / ds.solidAngleAbsCosTheta(in_index);
      A* ptr(data+in_index*ds.units());
      const A* const end(ptr + ds.units());
      for (;ptr!=end;ptr++) (*ptr) *= sact_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding out direction solid angle times cos(theta)
    // this requires (type A) *= (double) to be defined
  void divideAllByExSACT() {
    for (int out_index=0;out_index<ds.units();out_index++) {
      double sact_recip = 1.0 / ds.solidAngleAbsCosTheta(out_index);
      A* ptr(data+out_index);
      const A* const end(ptr + ds.unitsSquared());
      for (;ptr!=end;ptr+=ds.units()) (*ptr) *= sact_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding incident direction abs cos(theta)
    // this requires (type A) *= (double) to be defined
  void divideAllByInCT() {
    for (int in_index=0;in_index<ds.units();in_index++) {
      double ct_recip = 1.0 / fabs(ds.meanCosTheta(in_index));
      A* ptr(data+in_index*ds.units());
      const A* const end(ptr + ds.units());
      for (;ptr!=end;ptr++) (*ptr) *= ct_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by its corresponding exitant direction abs cos(theta)
    // this requires (type A) *= (double) to be defined
  void divideAllByExCT() {
    for (int out_index=0;out_index<ds.units();out_index++) {
      double ct_recip = 1.0 / fabs(ds.meanCosTheta(out_index));
      A* ptr(data+out_index);
      const A* const end(ptr + ds.unitsSquared());
      for (;ptr!=end;ptr+=ds.units()) (*ptr) *= ct_recip;
    }
    lazy_valid = false;
  }

    // divide each entry by the corresponding entry in dt for its in index
    // this requires (type A) /= (type F) to be defined
  template <typename F>
  void divideAtInIndices(const DirecTable<F,DS>& dt) { 
  
    if (!ds.isEqual(dt.direcStruct())) { jlog::ls << "Internal error " << __FUNCTION__ << " incompatible directional structure"; internalError(); }
  
    A* to(data);
    const F* div = dt.dataArray();
    const F* const div_end = dt.dataArray() + ds.units();

    for (;div!=div_end;div++) {
      const A* const end(to + ds.units());
      for (;to!=end;to++) *to /= *div; 
    }
    lazy_valid = false;
  }

    // divide each entry by the corresponding entry in dt for its out index
    // this requires (type A) /= (type F) to be defined
  template <typename F>
  void divideAtOutIndices(const DirecTable<F,DS>& dt) { 
  
    if (!ds.isEqual(dt.direcStruct())) { jlog::es << "Internal error " << __FUNCTION__ << " incompatible directional structure"; internalError(); }
    
    A* start(data);
    const A* const end(data + ds.unitsSquared());
    
    const F* div = dt.dataArray();
    const F* const div_end = dt.dataArray() + ds.units();

    for (;div!=div_end;div++,start++) {
      A* to(start);
      for (;to<end;to+=ds.units()) *to /= *div; 
    }
    lazy_valid = false;
  }

  void makeFullSymmetry();

    // probably is never used - depreceate ?
  void applyToAll(void(*func)(A&)) {
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while (ptr!=end) func(*(ptr++));
  }

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

  const A sumOfBackwardEntries() const;

  int direcToIndex(const Vec3D& direc_out) const { return ds.direcToIndex(direc_out); }
  Vec3D indexToDirec(int i) const { return ds.indexToDirec(i); }
 
  A& atIndices(int in_index, int out_index) { return data[in_index*ds.units() + out_index]; }
  const A& atIndices(int in_index, int out_index) const { return data[in_index*ds.units() + out_index]; }
 
  A& at(const Vec3D& in_direc, const Vec3D& out_direc) { 
    return atIndices(ds.direcToIndex(in_direc), ds.direcToIndex(out_direc)); }

  const A& at(const Vec3D& in_direc, const Vec3D& out_direc) const { 
    return atIndices(ds.direcToIndex(in_direc), ds.direcToIndex(out_direc)); }

  void direcTableFromInIndex(int in_index, DirecTable<A,DS>& dt) const;

  void translateInToOut(const DirecTable<A,DS>& dt_in, DirecTable<A,DS>& dt_out) const;
  //void translateInToOut(const DirecAdap<A,DS>& da_in, DirecAdap<A,DS>& da_out) const;
 
  //void translateOutToOut(const DirecTable<A,DS>& dt_in, DirecTable<A,DS>& dt_out) const;

  void extractOutData(int in_index, int rotate_pos, DirecTable<A,DS>& dt_out) const;
  void addToOutData(int in_index, int rotate_pos, const DirecTable<A,DS>& dt_out);
  
    // return minimum value in table
    // this requires (type A) < (type A) to be defined 
  const A min() const {
    const A* curr_min(data);
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while ((++ptr)!=end) if (*ptr < *curr_min) curr_min = ptr;
    return *curr_min;
  }

    // return maximum value in table
    // this requires (type A) > (type A) to be defined 
  const A max() const {
    const A* curr_max(data);
    A* ptr(data);
    const A* const end(data + ds.unitsSquared());
    while ((++ptr)!=end) if (*ptr > *curr_max) curr_max = ptr;
    return *curr_max;
  }
 
    // requires operator>> from a JTextStream to type A
  void read(JTextStream& str) {
    A* to(data);
    const A* const end(to + ds.unitsSquared());
    for (;to!=end;to++) str >> *to; 
    lazy_valid = false;
  }

    // requires operator<< to a JTextStream from type A
  void write(JTextStream& str) const {
    const A* from(data);
    const A* const end(from + ds.unitsSquared());
    for (;from!=end;from++) str << *from;
  }

  bool read(JDataStream& str, uns32 saved_version);
  bool readDirecStruct(JDataStream& str, uns32 saved_version);
  bool readData(JDataStream& str, uns32 saved_version);
  bool write(JDataStream& str) const;

};



// DirecDirecTable<A,DS>::makeFullSymmetry()

template <typename A, typename DS> 
void DirecDirecTable<A,DS>::makeFullSymmetry() {
 
    // if no symmetry data then quit
  if (ds.symmetricalTransNum()==0) return;

  const DirecDirecTable<A,DS> ddt(*this);
 
  for (int i0=0;i0<ds.units();i0++) {
    const int* index_list0 = ds.symmetricalIndices(i0);
    
    for (int i1=0;i1<ds.units();i1++) {
      const int* index_list1 = ds.symmetricalIndices(i1);
   
      A v(0);
      for (int j=0;j<ds.symmetricalTransNum();j++) v += ddt.atIndices(index_list0[j], index_list1[j]);
      v /= ds.symmetricalTransNum();
      atIndices(i0,i1) = v;
    }
  }
}



// DirecDirecTable::direcTableFromInIndex()

template <typename A, typename DS> 
void DirecDirecTable<A,DS>::direcTableFromInIndex(int in_index, DirecTable<A,DS>& dt) const {

    // reflection table out data from this in index
  const A* from(data + in_index*dt.units());
  const A* const from_end(from + ds.units());
    // start and end of radiance in array
  A* to(dt.dataArray());
    // copy entries
  for (;from!=from_end;from++,to++) *to = *from;
}



// DirecDirecTable::translateInToOut()

template <typename A, typename DS> 
void DirecDirecTable<A,DS>::translateInToOut(const DirecTable<A,DS>& dt_in, DirecTable<A,DS>& dt_out) const {

    // reflection table data
  const A* ptr(data);

    // start and end of radiance in array
  const A* from(dt_in.dataArray());
  const A* const from_end(from + ds.units());

    // loop through radiance in entries
  for (;from!=from_end;from++) {

      // start and end of radiance out array
    A* to(dt_out.dataArray());
    const A* const to_end(to + ds.units());

      // loop through out array adding contributions from this in direction
    for (;to!=to_end;to++,ptr++) (*to) += (*ptr) * (*from);
  }  
}



template <typename A, typename DS> 
void DirecDirecTable<A,DS>::extractOutData(int in_index, int rotate_pos, DirecTable<A,DS>& dt_out) const {

    // the in-index on the function table
  int ddt_in_index = ds.azimuthallyRotatedIndex(rotate_pos, in_index);
    // loop through indices of output structure
  for (int out_index=0; out_index<ds.units(); out_index++) {
      // the out-index on the function table
    int ddt_out_index = ds.azimuthallyRotatedIndex(rotate_pos, out_index);
      // set data in the output table
    dt_out.atIndex(out_index) = atIndices(ddt_in_index, ddt_out_index);
  }
}



template <typename A, typename DS> 
void DirecDirecTable<A,DS>::addToOutData(int in_index, int rotate_pos, const DirecTable<A,DS>& dt_out) {

    // the in-index on the function table
  int ddt_in_index = ds.azimuthallyRotatedIndex(rotate_pos, in_index);
    // loop through indices of output structure
  for (int out_index=0; out_index<ds.units(); out_index++) {
      // the out-index on the function table
    int ddt_out_index = ds.azimuthallyRotatedIndex(rotate_pos, out_index);
      // set data in the output table
    atIndices(ddt_in_index, ddt_out_index) += dt_out.atIndex(out_index);
  }
} 



// DirecDirecTable::translateOutToOut()
// this only works for full (spherical or cube) reflection direction structures
// the reflection table is set up for translateing an incoming radiance integral to outgoing radiance
// so for e.g. the value atIndices(i,i) represents backscatter
// this function instead translates outgoing radiance to outgoing radiance
// so the value atIndices(i,i) is foward scatter
// note entries must be multipliedAllByInSA()/4pi (for volume scattering) to correct the function to give L -> L

//template <typename A, typename DS> 
//void DirecDirecTable<A,DS>::translateOutToOut(const DirecTable<A,DS>& dt_out0, DirecTable<A,DS>& dt_out1) const {

//for (int in_index=0; in_index<ds.units(); in_index++) {
////int neg_in_index = ds.negatedIndex(in_index);
 
//  for (int out_index=0; out_index<ds.units(); out_index++) {
//     dt_out1.atIndex(out_index).addMultiple(atIndices(in_index, out_index), dt_out0.atIndex(ds.negatedIndex(in_index)));
//   }
// }  
//}



// DirecDirecTable::translateInToOut()

// *** probably no point in implementing this since it will be very inefficient and quite hard to do ***

//template <typename A, typename DS> 
//void DirecDirecTable<A,DS>::translateInToOut(const DirecAdap<A,DS>& da_in, DirecAdap<A,DS>& da_ex) const {

  //static A zero_data(0);
  //int leaves;
  /*
    // first deal with special indices in the in structure (i.e. end caps)
    // non adaptive indices in the out structure will be dealt with automatically below
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

      // this non-adaptive node
    const A& vin = da_in.nonAdaptiveNode(i);
      // if data in isn't zero
    if (vin != zero_data) {
        // go through all out indexes
      for (int out_index=0;out_index<ds.units();out_index++) {
	  // the multiplier from this in to out
	const A& mult_fac = atIndices(ds.nonAdapIndex(i), out_index);
 	  // if the multiplier is not zero
	if (mult_fac != zero_data) {
	    // find the out data leaf in the adaptive table, creating down to max_depth if required
	  DA_NodeSeekRW<A,DS> node_seek(da_out, index, da_out.growDepth(), da_out.maxDepth());
	    // the data at the leaf
	  A& data = *node_seek.data();
	    // add on this data scaled for repeated adding because it will be added again if leaves>0
	  data.addMultiple(vin, mult_fac, 1.0/node_seek.leaves());
	}
      }
    }
  }
    // loop through all leaves on the incoming structure
  DA_LeafIter<A,DS> iter(da_in);
  DA_Node<A>* ptr = iter.first();
  while (ptr) {

      // the incoming data at this leaf
    A vin(ptr->data());
      // divide by the solid angle at this leaf depth so can make into a maximum depth value later
    vin /= iter.solidAngle();
 
      // loop through all out indices
    for (int out_index=0;out_index<ds.units();out_index++) {

        // loop through the bottom level leaves indices represented by this node
      int in_index = iter.firstIndex();
      while (in_index >= 0) {

	  // the multiplier for this pair of indices
	const A& mult_fac = atIndices(in_index, out_index);
	  // if multiplier is not zero
	if (mult_fac != zero_data) {
	    // get the out data and number of bottom leaves it represents  
	  A& data = da_out.findOrCreateAtIndex(out_index, leaves, AdapControl::penetrationDepth(), AdapControl::structureFixedDepth());
	    // add on this data scaled for repeated adding
	  data.addMultiple(vin, mult_fac, ds.solidAngle(in_index) / leaves);
	}
	  // next in index represented by this node
	in_index = iter.nextIndex();
      }
    }

    ptr = iter.next();
  }
  */
//}



// DirecDirecTable::lazyCalc() [private]

template <typename A, typename DS> 
void DirecDirecTable<A,DS>::lazyCalc() const {

  entry_sum = A(0);

  A* ptr(data);
  const A* const end(data + ds.unitsSquared());
  while (ptr!=end) entry_sum += (*(ptr++));

  lazy_valid = true;
}



// DirecDirecTable::sumOfBackwardEntries()

template <typename A, typename DS> 
const A DirecDirecTable<A,DS>::sumOfBackwardEntries() const {

  A sum(0);

  return sum;
}



/*! Reads in a data from data stream into a constructed DirecDirecTable. */

template <typename A, typename DS>
bool DirecDirecTable<A,DS>::read(JDataStream& str, uns32 saved_version) {
    // read in the directional partition
  ds.read(str);
    // ensure memory is allocated
  allocate(ds.unitsSquared());
    // read in the data
  A* to(data);
  const A* const end(to + ds.unitsSquared());
  for (;to!=end;to++) if (!readFromDataStream(*to, str, saved_version)) return false; 
    // calculated values are no longer valid
  lazy_valid = false;
  return true;
}



template <typename A, typename DS>
bool DirecDirecTable<A,DS>::readDirecStruct(JDataStream& str, uns32 saved_version) {
    // read in the directional partition
  ds.read(str);
    // ensure memory is allocated
  allocate(ds.unitsSquared());
  return true;
}



template <typename A, typename DS>
bool DirecDirecTable<A,DS>::readData(JDataStream& str, uns32 saved_version) {
    // read in the data
  A* to(data);
  const A* const end(to + ds.unitsSquared());
  for (;to!=end;to++) if (!readFromDataStream(*to, str, saved_version)) return false; 
    // calculated values are no longer valid
  lazy_valid = false;
  return true;
}



/*! Writes data to a data stream. */

template <typename A, typename DS>
bool DirecDirecTable<A,DS>::write(JDataStream& str) const {
    // write the directional partiton
  ds.write(str);
    // write the data
  const A* from(data);
  const A* const end(from + ds.unitsSquared());
  for (;from!=end;from++) if (!writeToDataStream(*from, str)) return false;
  return true;
}



} } // namespace jude::table

#endif // _JUDE_TABLE_DIRECDIRECTABLE_H

