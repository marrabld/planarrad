
// aqrad
// direcdirec_sparse.h
// (c) John Hedley 2003

#ifndef _DIRECDIRECSPARSE_H_JDH
#define _DIRECDIRECSPARSE_H_JDH

#include "dds_node.h"
#include "jdatastream.h"
#include "direcadap.h"


template <typename A, typename DS> class DDS_LeafIter;
template <typename A, typename DS> class DDS_NodeSeekRO;
template <typename A, typename DS> class DDS_NodeSeekRW;



// DirecDirecSparse

// more efficient form of a BRDF reflectance structure
// this version works like a SparseMatrix type with a 4-dimensional hierachical node structure
// currently if a node is not defined then a common base value is returned (e.g. zero)
// very efficent for specular reflectance
// a future enhancement of this could be a hierachical adaptive structure where each node level adds
// more and more precise data - maybe wavelet based, or spherical harmonics ?

template <typename A, typename DS> 
class DirecDirecSparse {

public:

    // if these values are changed some routines will be broken!
  enum { input_x_bit = 1, 
	 input_y_bit = 2,
	 output_x_bit = 4,
	 output_y_bit = 8 };

  struct Pos_Data {
    DDS_Node<A>* node;
    int child;
    int width_x;
    int width_y;
  };

  DS ds;

  A base_data;  // the value used for all undefined locations
  bool base_data_is_null;

  DDS_Node<A>** top_node; // the top of the tree

    // used for special indices that can't be subdivided, e.g. end caps on quad spheres
  DirecTable<A,DS>** non_adaptive_in;
  DirecTable<A,DS>** non_adaptive_out;

  mutable bool lazy_valid;
  mutable A entry_sum;

  DDS_Node<A>* topNode(int i) { return top_node[i]; }
  const DDS_Node<A>* topNode(int i) const { return top_node[i]; }

  void setBaseData(const A& a) { 
    base_data = a; 
    base_data_is_null = (base_data == A(0));
  }
 
  template <typename F>
  void translateInToOut_BaseData(const DirecTable<F,DS>& data_in, DirecTable<F,DS>& data_out) const;
 
  void lazyCalc() const;

public:

  DirecDirecSparse<A,DS>(const DS& ds);
  DirecDirecSparse<A,DS>(const DS& ds, JDataStream& str); 
  DirecDirecSparse<A,DS>(const DS& ds, JTextStream& str); 
  DirecDirecSparse<A,DS>(const DirecDirecSparse<A,DS>& dds);
  DirecDirecSparse(DumpStreamIn& str);

  ~DirecDirecSparse();

  void dumpWrite(DumpStreamOut& str) const;
 
  const A& baseData() const { return base_data; }

  const bool baseDataIsNull() const { return base_data_is_null; }

  const DS& direcStruct() const { return ds; }

  void copy(const DirecDirecSparse<A,DS>& dds);
  void copyInvertedIn(const DirecDirecSparse<A,DS>& dds);
 
  void initAll(const A& a);
 
  template <typename F>
  void add(const F& f);

  template <typename F>
  void subtract(const F& f);

  template <typename F>
  void multiply(const F& f);

  template <typename F>
  void divide(const F& f);
 
  template <typename F>
  void add(const DirecDirecSparse<F,DS>& ds);

  template <typename F>
  void subtract(const DirecDirecSparse<F,DS>& ds);

  template <typename F>
  void divide(const DirecDirecSparse<F,DS>& ds);

  void multiplyAllByInSA();
  void multiplyAllByOutSA();
  void multiplyAllByInSACT();
  void multiplyAllByOutSACT();

  void divideAllByInSA();
  void divideAllByOutSA();
  void divideAllByInSACT();
  void divideAllByOutSACT();

  void divideAllByInCT();
  void divideAllByOutCT();
 
  template <typename F>
  void divideAtInIndices(const DirecTable<F,DS>& dt);

  template <typename F>
  void divideAtOutIndices(const DirecTable<F,DS>& dt);

  const A min() const;
  const A max() const;

  void testCall();
  void makeFullSymmetry();

    // probably is never used - depreceate ?
  void applyToAll(void(*func)(A&));

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

  int direcToIndex(const Vector3D& direc_out) const { return ds.direcToIndex(direc_out); }
  Vector3D indexToDirec(int i) const { return ds.indexToDirec(i); }
 
  A& atIndices(int in_index, int out_index);
  const A& atIndices(int in_index, int out_index) const;

  A& at(const Vector3D& in_direc, const Vector3D& out_direc) { 
    return atIndices(ds.direcToIndex(in_direc), ds.direcToIndex(out_direc)); }

  const A& at(const Vector3D& in_direc, const Vector3D& out_direc) const { 
    return atIndices(ds.direcToIndex(in_direc), ds.direcToIndex(out_direc)); }

  void direcTableFromInIndex(int in_index, DirecTable<A,DS>& dt) const;

  template <typename F>
  void translateInToOut(const DirecTable<F,DS>& data_in, DirecTable<F,DS>& data_out) const;

  template <typename F>
  void translateInToOut(const DirecAdap<F,DS>& da_in, DirecAdap<F,DS>& da_out) const {
    internalError("DirecDirecSparse::translateInToOut() on DirecAdap not implemented");
  }

  void translateInToOut_Symmetric(const DirecTable<A,DS>& data_in, DirecTable<A,DS>& data_out) const;

  void translateInToOut_Symmetric(const DirecAdap<A,DS>& data_in, DirecAdap<A,DS>& data_out) const {
    internalError("DirecDirecSparse::translateInToOut_Symmetric() on DirecAdap not implemented");
  }

  //template <typename F>
    //void translateOutToOut(const DirecTable<F,DS>& data_out0, DirecTable<F,DS>& data_out1) const;

  //void translateOutToOut_Symmetric(const DirecTable<A,DS>& data_in, DirecTable<A,DS>& data_out) const;
 

  void read(JDataStream& str);
  void write(JDataStream& str) const;
  void read(JTextStream& str) { }
  void write(JTextStream& str) const { }

  friend class DDS_LeafIter<A,DS>;
  friend class DDS_NodeSeekRO<A,DS>;
  friend class DDS_NodeSeekRW<A,DS>;
};



// mutually dependent classes

#include "dds_leafiter.h"
#include "dds_nodeseekro.h"
#include "dds_nodeseekrw.h"



// DirecDirecSparse::(constructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::DirecDirecSparse(const DS& ds) :
  ds(ds),
  base_data(0),
  base_data_is_null(true),
  top_node(new DDS_Node<A>*[ds.adapTopNodesSQ()]), 
  non_adaptive_in(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  non_adaptive_out(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  lazy_valid(false) { 

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i] = new DirecTable<A,DS>(ds);
    non_adaptive_out[i] = new DirecTable<A,DS>(ds);
  }

  for (int i=0;i<ds.adapTopNodesSQ();i++) top_node[i] = 0;
}



// DirecDirecSparse::(constructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::DirecDirecSparse(const DS& ds, JDataStream& str) :
  ds(ds),
  base_data(0),
  base_data_is_null(true),
  top_node(new DDS_Node<A>*[ds.adapTopNodesSQ()]), 
  non_adaptive_in(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  non_adaptive_out(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  lazy_valid(false) { 

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i] = new DirecTable<A,DS>(ds);
    non_adaptive_out[i] = new DirecTable<A,DS>(ds);
  }

  read(str); 
}



// DirecDirecSparse::(constructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::DirecDirecSparse(const DS& ds, JTextStream& str) :
  ds(ds),
  top_node(new DDS_Node<A>*[ds.adapTopNodesSQ()]), 
  non_adaptive_in(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  non_adaptive_out(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  lazy_valid(false) { 

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i] = new DirecTable<A,DS>(ds);
    non_adaptive_out[i] = new DirecTable<A,DS>(ds);
  }

  read(str); 
} 



// DirecDirecSparse::(constructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::DirecDirecSparse(const DirecDirecSparse<A,DS>& dds) :
  ds(dds.direcStruct()),
  top_node(new DDS_Node<A>*[ds.adapTopNodesSQ()]), 
  non_adaptive_in(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  non_adaptive_out(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]) { 

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i] = new DirecTable<A,DS>(ds);
    non_adaptive_out[i] = new DirecTable<A,DS>(ds);
  }

  copy(dds);
}
 


// DirecDirecSparse::(constructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::DirecDirecSparse(DumpStreamIn& str) :
  ds(str), 
  top_node(new DDS_Node<A>*[ds.adapTopNodesSQ()]), 
  non_adaptive_in(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]),
  non_adaptive_out(new DirecTable<A,DS>*[ds.nonAdapIndexNum()]) {

  read(str);  
  str >> lazy_valid;
  str >> entry_sum;
}



// DirecDirecSparse::(destructor)

template <typename A, typename DS>
DirecDirecSparse<A,DS>::~DirecDirecSparse() {

  for (int i=0;i<ds.adapTopNodesSQ();i++) {
    if (top_node[i]) top_node[i]->destroyChildren(ds.adapMaxDepth()-1);
    delete top_node[i];
  }

  delete[] top_node;

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    delete non_adaptive_in[i];
    delete non_adaptive_out[i];
  }
  delete[] non_adaptive_in;
  delete[] non_adaptive_out;
}



// DirecDirecSparse<A,DS>::dumpWrite()

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::dumpWrite(DumpStreamOut& str) const { 
  ds.dumpWrite(str); 
  write(str);
  str << lazy_valid;
  str << entry_sum;
}



// DirecDirecSparse::copy()

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::copy(const DirecDirecSparse<A,DS>& dds) {

  if (!ds.isEqual(dds.direcStruct())) internalError("%s:%s incompatible directional structure",__FILE__,__LINE__);

    // copy base data
  setBaseData(dds.base_data);
    // copy non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->copy(*dds.non_adaptive_in[i]);  
    non_adaptive_out[i]->copy(*dds.non_adaptive_out[i]);  
  }
    // adaptive nodes
  for (int i=0;i<ds.adapTopNodesSQ();i++) {
      // recursively create nodes
    if (dds.topNode(i)) top_node[i] = new DDS_Node<A>(*dds.topNode(i), ds.adapMaxDepth()-1);
    else top_node[i] = 0;
  }

  lazy_valid = dds.lazy_valid;
  entry_sum = dds.entry_sum;
}



// DirecDirecSparse::copyInvertedIn()

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::copyInvertedIn(const DirecDirecSparse<A,DS>& dds) {

  if (!ds.isEqual(dds.direcStruct())) internalError("%s:%s incompatible directional structure",__FILE__,__LINE__);
  if (!ds.isFull()) internalError("%s:%s non-full directional structure",__FILE__,__LINE__);

    // copy base data
  setBaseData(dds.base_data);
    // copy non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->copy(*dds.non_adaptive_in[direcStruct().negatedIndex(i)]);  
    non_adaptive_out[i]->copyInverted(*dds.non_adaptive_out[i]);  
  }

    // walk the leaf nodes of the other structure
  DDS_LeafIter<A,DS> iter(dds);
  A* ptr = iter.first();
  while (ptr) {
      // create the entry at the inverted in-index position
    atIndices(direcStruct().negatedIndex(iter.inIndex()), iter.outIndex()) = (*ptr);
    //atIndices(iter.inIndex(), iter.outIndex()) = (*ptr);
    ptr = iter.next();
  }

  lazy_valid = dds.lazy_valid;
  entry_sum = dds.entry_sum;
}



// DirecDirecSparse::initAll()
// this requires (type A) = (type A) to be defined 

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::initAll(const A& a) {

    // destroy any nodes
  for (int i=0;i<ds.adapTopNodesSQ();i++) {
    if (top_node[i]) top_node[i]->destroyChildren(ds.adapMaxDepth()-1);
    delete top_node[i];
    top_node[i] = 0;
  }
    // base data is the new data
  setBaseData(a);
    // initialise and non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->initAll(a); 
    non_adaptive_out[i]->initAll(a); 
  }
    // have to recalculate entry sum
  lazy_valid = false;
}



// DirecDirecSparse::multiply()
// this requires (type A) += (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::add(const F& f) {

    // add to the base data and check if is null
  base_data += f;
  base_data_is_null = (base_data == A(0));
    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->add(f);  
    non_adaptive_out[i]->add(f);  
  }
    // walk the leaf nodes doing the addition
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    (*ptr) += f;
    ptr = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::multiply()
// this requires (type A) -= (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::subtract(const F& f) {

    // subtract from the base data and check if is null
  base_data -= f;
  base_data_is_null = (base_data == A(0));
    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->subtract(f);  
    non_adaptive_out[i]->subtract(f);  
  }
    // walk the leaf nodes doing the addition
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    (*ptr) -= f;
    ptr = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::multiply()
// this requires (type A) *= (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::multiply(const F& f) {
    // multply the base data - if it was null will still be so
  base_data *= f;
    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->multiply(f);  
    non_adaptive_out[i]->multiply(f);  
  }
    // walk the leaf nodes doing the multiply
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    //printf("here %d %d t %d\n",iter.inIndex(),iter.outIndex(),iter.currTop());
    //ptr->display();
    (*ptr) *= f;
    ptr = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divide()
// this requires (type A) /= (type F) to be defined
// note: in general probably more efficent to calculate 1/f and then do a multiply() if possible

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::divide(const F& f) {
    // divide the base data - if it was null will still be so
  base_data /= f;
    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(f);  
    non_adaptive_out[i]->divide(f);  
  }
    // walk the leaf nodes doing the divide
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    (*ptr) /= f;
    ptr = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::add()
// add together two sparse structures (*this) will have leaves added as required
// this requires (type A) += (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::add(const DirecDirecSparse<F,DS>& dds) {

  if (!ds.isEqual(dds.direcStruct())) internalError("__FUNCTION__ : incompatible directional structure");

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->add(*dds.non_adaptive_in[i]);  
    non_adaptive_out[i]->add(*dds.non_adaptive_out[i]);
  }

    // walk the leaf nodes of the other structure
  DDS_LeafIter<A,DS> iter(dds);
  A* ptr = iter.first();

    // is quickest if we can ignore the base data on the other structure
  if (dds.baseDataIsNull()) {

    while (ptr) {
        // add on to leaf at this structure, creating if necessary
      atIndices(iter.inIndex(), iter.outIndex()) += (*ptr);
        // next leaf on other structure
      ptr = iter.next();
    }

  } else {
 
    while (ptr) {
        // add on to leaf at this structure, creating if necessary
      atIndices(iter.inIndex(), iter.outIndex()) += (*ptr) - dds.baseData();
        // next leaf on other structure
      ptr = iter.next();
    }

      // add on other base data everywhere
    add(dds.baseData());
  }

  lazy_valid = false;
}



// DirecDirecSparse::subtract()
// subtract another sparse structure from this (*this) will have leaves added as required
// this requires (type A) -= (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::subtract(const DirecDirecSparse<F,DS>& dds) {

  if (!ds.isEqual(dds.direcStruct())) internalError("__FUNCTION__ : incompatible directional structure");

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->subtract(*dds.non_adaptive_in[i]);  
    non_adaptive_out[i]->subtract(*dds.non_adaptive_out[i]);
  }

    // walk the leaf nodes of the other structure
  DDS_LeafIter<A,DS> iter(dds);
  A* ptr = iter.first();

    // is quickest if we can ignore the base data on the other structure
  if (dds.baseDataIsNull()) {

    while (ptr) {
        // add on to leaf at this structure, creating if necessary
      atIndices(iter.inIndex(), iter.outIndex()) -= (*ptr);
        // next leaf on other structure
      ptr = iter.next();
    }

  } else {
 
    while (ptr) {
        // add on to leaf at this structure, creating if necessary
      atIndices(iter.inIndex(), iter.outIndex()) -= (*ptr) - dds.baseData();
        // next leaf on other structure
      ptr = iter.next();
    }

      // subtract other base data everywhere
    subtract(dds.baseData());
  }

  lazy_valid = false;
}



// DirecDirecSparse::add()
// divide two all nodes on this structure bu the values in (dds)
// this requires (type A) /= (type F) to be defined

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::divide(const DirecDirecSparse<F,DS>& dds) {

  if (!ds.isEqual(dds.direcStruct())) internalError("__FUNCTION__ : incompatible directional structure");

    // is quickest if we can ignore the base data on the other structure
  if (!dds.baseDataIsNull()) internalError("__FUNCTION__ : base data not null");

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->add(*dds.non_adaptive_in[i]);  
    non_adaptive_out[i]->add(*dds.non_adaptive_out[i]);
  }

    // walk the leaf nodes of this structure
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();

  while (ptr) {
      // add on to leaf at this structure, creating if necessary
    (*ptr) /= dds.atIndices(iter.inIndex(), iter.outIndex());
      // next leaf
    ptr = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::min()
// this requires (type A) += (type F) to be defined

template <typename A, typename DS>
const A DirecDirecSparse<A,DS>::min() const {

    // add to the base data and check if is null
  A curr_min(base_data);

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

    A t(non_adaptive_in[i]->min()); 
    if (t < curr_min) curr_min = t; 

    t = non_adaptive_out[i]->min(); 
    if (t < curr_min) curr_min = t;
  }

    // walk any leaf nodes to check if any is lower
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    if (*ptr < curr_min) curr_min = *ptr;
    ptr = iter.next();
  }

  return curr_min;
}



// DirecDirecSparse::max()
// this requires (type A) > (type A) to be defined

template <typename A, typename DS>
const A DirecDirecSparse<A,DS>::max() const {

    // add to the base data and check if is null
  A curr_max(base_data);

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

    A t(non_adaptive_in[i]->max()); 
    if (t > curr_max) curr_max = t; 

    t = non_adaptive_out[i]->max(); 
    if (t > curr_max) curr_max = t;
  }

    // walk any leaf nodes to check if any is lower
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    if (*ptr > curr_max) curr_max = *ptr;
    ptr = iter.next();
  }

  return curr_max;
}



// DirecDirecSparse<A,DS>::divideByNodeRepeats()
/*
template <typename A, typename DS> 
void DirecDirecSparse<A,DS>::testCall() {

  ReflecSparse<DS> rc(ds);
  rc.makeAll(SpectralData(0));
  for (int i=0;i<ds.units();i++) {
    for (int j=0;j<ds.units();j++) {
      rc.addToReflectance(i,j,SpectralData(1));
    }
  }
  rc.display();
  
  divide(ds.symmetricalIndicesNum());
  return;

    // if isn't internally symmetric then doesn't make sense
  if (!ds.fullSymmetric()) return;

  DirecDirecSparse<int,DS> dc(ds);
  dc.initAll(0);
  for (int i=0;i<ds.units();i++) {
    for (int j=0;j<ds.units();j++) {
      dc.atIndices(i,j)++;
    }
  }
  
  divide(dc);
  
}
*/


// DirecDirecSparse<A,DS>::makeFullSymmetry()

template <typename A, typename DS> 
void DirecDirecSparse<A,DS>::makeFullSymmetry() {
  
    // if no symmetry data then quit
  if (ds.symmetricalTransNum()==0) return;

  const DirecDirecSparse<A,DS> dds(*this);

  for (int i0=0;i0<ds.units();i0++) {
    const int* index_list0 = ds.symmetricalIndices(i0);
  
    for (int i1=0;i1<ds.units();i1++) {
      const int* index_list1 = ds.symmetricalIndices(i1);
  
      A v(0);
      for (int j=0;j<ds.symmetricalTransNum();j++) v += dds.atIndices(index_list0[j], index_list1[j]);
      v /= ds.symmetricalTransNum();
      if (v != base_data) atIndices(i0,i1) = v;
    }
  }
    // probably shouldn't effect entry sum, but better to be safe 
  lazy_valid = false;
}



// DirecDirecSparse::direcTableFromInIndex()

template <typename A, typename DS> 
void DirecDirecSparse<A,DS>::direcTableFromInIndex(int in_index, DirecTable<A,DS>& dt) const {
    // start and end of radiance in array
  A* to(dt.dataArray());
    // copy entries
  for (int out_index=0;out_index<ds.units();out_index++) *(to++) = atIndices(in_index, out_index);
}



// DirecDirecSparse::lazyCalc() [private]

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::lazyCalc() const {

  entry_sum = A(0);

    // deal with non-adaptive nodes
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    entry_sum += non_adaptive_in[i]->sumOfEntries();  
    entry_sum += non_adaptive_out[i]->sumOfEntries();  
  }
    // walk the leaf nodes
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();

    // quickest way is if we can ignore base_data
  if (base_data_is_null) {

    while (ptr) {
      entry_sum += (*ptr);
      ptr = iter.next();
    }

  } else {

      // this is the total of possible entires in the adaptive section 
    int count = (ds.units()-ds.nonAdapIndexNum());
    count = count * count;

    while (ptr) {
      entry_sum += (*ptr);
      count--;
      ptr = iter.next();
    }

      // add on the base_data for all the unseen entries
    entry_sum += base_data * count;
  }

  lazy_valid = true;
}



// DirecDirecSparse::atIndices()
// return the data at (in_index) and (out_index)
// this const version does not need to create any new nodes

template <typename A, typename DS>
const A& DirecDirecSparse<A,DS>::atIndices(int in_index, int out_index) const {

    // deal with directions into caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    if (ds.nonAdapIndex(i) == in_index) return non_adaptive_in[i]->atIndex(out_index);  
    if (ds.nonAdapIndex(i) == out_index) return non_adaptive_out[i]->atIndex(in_index);  
  }

  DDS_NodeSeekRO<A,DS> node_seek(*this, in_index, out_index);

  return node_seek.data();
}



// DirecDirecSparse::atIndices()
// return the data at (in_index) and (out_index)
// this non-const version creates new nodes as required down to the bottom level

template <typename A, typename DS>
A& DirecDirecSparse<A,DS>::atIndices(int in_index, int out_index) {

    // deal with directions into caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    if (ds.nonAdapIndex(i) == in_index) return non_adaptive_in[i]->atIndex(out_index);  
    if (ds.nonAdapIndex(i) == out_index) return non_adaptive_out[i]->atIndex(in_index);  
  }

  DDS_NodeSeekRW<A,DS> node_seek(*this, in_index, out_index);

  return node_seek.data();
}



// DirecDirecSparse::translateInToOut()
// applies reflection function to integral of incoming radiance 
// and adds to outgoing radiance structure
// note that this does not clear the outgoing radiance data to zero first
// fast implementation traverses tree
// requires F.addMultiple(F, A)

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::translateInToOut(const DirecTable<F,DS>& data_in, DirecTable<F,DS>& data_out) const {
 
    // quickest way is if we can ignore base_data
  if (!base_data_is_null) return translateInToOut_BaseData(data_in, data_out);

    // loop through special indicies, e.g. end caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

      // do all out indices for this non-adaptive index as an in index
    const F& ci = data_in.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_in = *non_adaptive_in[i];
    for (int out_index=0;out_index<ds.units();out_index++) {
      data_out.atIndex(out_index).addMultiple(ci, na_in.atIndex(out_index));
    }

      // do all in indices for this non-adaptive index as an out index
      // the safest thing to do with the repeat in_index out_index combinations is to explicity avoid them
      // since what is in the multiplier data may not be zero
    F& co = data_out.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_out = *non_adaptive_out[i];
    for (int in_index=0;in_index<ds.units();in_index++) {
      bool repeat = false;
      for (int j=0;j<ds.nonAdapIndexNum();j++) repeat = repeat || (in_index == ds.nonAdapIndex(j));
      if (!repeat) co.addMultiple(data_in.atIndex(in_index), na_out.atIndex(in_index));
    } 
  }

    // walk the leaf nodes of the sparse data
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();    
  while (ptr) {
    data_out.atIndex(iter.outIndex()).addMultiple(data_in.atIndex(iter.inIndex()), *ptr);
    ptr = iter.next();
  }
}



// DirecDirecSparse::translateOutToOut()
/*
template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::translateOutToOut(const DirecTable<F,DS>& data_out0, DirecTable<F,DS>& data_out1) const {
 
    // quickest way is if we can ignore base_data
  if (!base_data_is_null) internalError("DirecDirecSparse::translateInToOut() !base_data_is_null");

    // loop through special indicies, e.g. end caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    
      // do all out indices for this non-adaptive index as an in index
    const F& ci = data_in.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_in = *non_adaptive_in[i];
    for (int out_index=0;out_index<ds.units();out_index++) {
      data_out.atIndex(out_index).addMultiple(ci, na_in.atIndex(out_index));
    }

      // do all in indices for this non-adaptive index as an out index
      // the safest thing to do with the repeat in_index out_index combinations is to explicity avoid them
      // since what is in the multiplier data may not be zero
    F& co = data_out.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_out = *non_adaptive_out[i];
    for (int in_index=0;in_index<ds.units();in_index++) {
      bool repeat = false;
      for (int j=0;j<ds.nonAdapIndexNum();j++) repeat = repeat || (in_index == ds.nonAdapIndex(j));
      if (!repeat) co.addMultiple(data_in.atIndex(in_index), na_out.atIndex(in_index));
    } 
    
  }

    // walk the leaf nodes of the sparse data
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();    
  while (ptr) {
    data_out1.atIndex(iter.outIndex()).addMultiple(data_out0.atIndex(ds.negatedIndex(iter.inIndex())), *ptr);
    ptr = iter.next();
  }
}
*/


// DirecDirecSparse::translateInToOut_Symmetric()
// this version should be used when the data structure is symmetrically collapsed
// then each node works several equivalent in-out index pairs
// the data is set up so that this works right i.e. esp. the values for repeated node pairs are stored halved so works out correct
// only works with base_data = 0

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::translateInToOut_Symmetric(const DirecTable<A,DS>& data_in, DirecTable<A,DS>& data_out) const {

  if (!base_data_is_null) internalError("DirecDirecSparse::translateInToOut_Symmetric() and base_data != 0");

    // loop through special indicies, e.g. end caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

      // do all out indices for this non-adaptive index as an in index
    const A& ci = data_in.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_in = *non_adaptive_in[i];
    for (int out_index=0;out_index<ds.units();out_index++) {
      data_out.atIndex(out_index).addMultiple(ci, na_in.atIndex(out_index));
    }

      // do all in indices for this non-adaptive index as an out index
      // the safest thing to do with the repeat in_index out_index combinations is to explicity avoid them
      // since what is in the multiplier data may not be zero
    A& co = data_out.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_out = *non_adaptive_out[i];
    for (int in_index=0;in_index<ds.units();in_index++) {
      bool repeat = false;
      for (int j=0;j<ds.nonAdapIndexNum();j++) repeat = repeat || (in_index == ds.nonAdapIndex(j));
      if (!repeat) co.addMultiple(data_in.atIndex(in_index), na_out.atIndex(in_index));
    } 
  }

    // walk the leaf nodes of the sparse data
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();    
  while (ptr) {
 
    const int* in_index_tab = ds.symmetricalIndices(iter.inIndex());
    const int* out_index_tab = ds.symmetricalIndices(iter.outIndex());
     
    for (int i=0;i<ds.symmetricalTransNum();i++) {
      data_out.atIndex(out_index_tab[i]).addMultiple(data_in.atIndex(in_index_tab[i]), *ptr);
    }

    ptr = iter.next();
  }
}



// DirecDirecSparse::translateInToOut_Symmetric()
// this version should be used when the data structure is symmetrically collapsed
// then each node works several equivalent in-out index pairs
// the data is set up so that this works right i.e. esp. the values for repeated node pairs are stored halved so works out correct
// only works with base_data = 0
/*
template <typename A, typename DS>
void DirecDirecSparse<A,DS>::translateOutToOut_Symmetric(const DirecTable<A,DS>& data_out0, DirecTable<A,DS>& data_out1) const {

  if (!base_data_is_null) internalError("DirecDirecSparse::translateOutToOut_Symmetric() !base_data_is_null");

    // loop through special indicies, e.g. end caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    
      // do all out indices for this non-adaptive index as an in index
    const A& ci = data_in.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_in = *non_adaptive_in[i];
    for (int out_index=0;out_index<ds.units();out_index++) {
      data_out.atIndex(out_index).addMultiple(ci, na_in.atIndex(out_index));
    }

      // do all in indices for this non-adaptive index as an out index
      // the safest thing to do with the repeat in_index out_index combinations is to explicity avoid them
      // since what is in the multiplier data may not be zero
    A& co = data_out.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_out = *non_adaptive_out[i];
    for (int in_index=0;in_index<ds.units();in_index++) {
      bool repeat = false;
      for (int j=0;j<ds.nonAdapIndexNum();j++) repeat = repeat || (in_index == ds.nonAdapIndex(j));
      if (!repeat) co.addMultiple(data_in.atIndex(in_index), na_out.atIndex(in_index));
    } 
    
  }

    // walk the leaf nodes of the sparse data
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();    
  while (ptr) {
 
    const int* in_index_tab = ds.symmetricalIndices(iter.inIndex());
    const int* out_index_tab = ds.symmetricalIndices(iter.outIndex());
     
    for (int i=0;i<ds.symmetricalTransNum();i++) {
      data_out1.atIndex(out_index_tab[i]).addMultiple(data_out0.atIndex(ds.negatedIndex(in_index_tab[i])), *ptr);
    }

    ptr = iter.next();
  }
}

*/

// DirecDirecSparse::translateInToOut_BaseData()
// this version is used when base data is not zero

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::translateInToOut_BaseData(const DirecTable<F,DS>& data_in, DirecTable<F,DS>& data_out) const {
 
    // loop through special indicies, e.g. end caps for quad spheres
  for (int i=0;i<ds.nonAdapIndexNum();i++) {

      // do all out indices for this non-adaptive index as an in index
    const F& ci = data_in.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_in = *non_adaptive_in[i];
    for (int out_index=0;out_index<ds.units();out_index++) {
      data_out.atIndex(out_index).addMultiple(ci, na_in.atIndex(out_index) - base_data);
    }

      // do all in indices for this non-adaptive index as an out index 
      // the safest thing to do with the repeat in_index out_index combinations is to explicity avoid them
      // since what is in the multiplier data may not be zero
    F& co = data_out.atIndex(ds.nonAdapIndex(i));
    const DirecTable<A,DS>& na_out = *non_adaptive_out[i];
    for (int in_index=0;in_index<ds.units();in_index++) {
      bool repeat = false;
      for (int j=0;j<ds.nonAdapIndexNum();j++) repeat = repeat || (in_index == ds.nonAdapIndex(j));
      if (!repeat) co.addMultiple(data_in.atIndex(in_index), na_out.atIndex(in_index) - base_data);
    } 
  }

    // walk the leaf nodes of the sparse data
  DDS_LeafIter<A,DS> iter(*this);
  A* ptr = iter.first();
  while (ptr) {
    data_out.atIndex(iter.outIndex()).addMultiple(data_in.atIndex(iter.inIndex()), (*ptr) - base_data);
    ptr = iter.next();
  }
    // then add on a multiple of base data to all
  F f(0);
  //data_in.sumOfEntries() * base_data);
  //data_out.add(a);
  f.addMultiple(data_in.sumOfEntries(), base_data);
  data_out.add(f);
}



// DirecDirecSparse::mutiplyAllByInSA()
// this requires (type A) *= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding in-direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::multiplyAllByInSA() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->multiply(ds.solidAngle(ds.nonAdapIndex(i)));
    non_adaptive_out[i]->multiplyAllBySA();
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) *= ds.solidAngle(iter.inIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::multiplyAllByOutSA()
// this requires (type A) *= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding out direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::multiplyAllByOutSA() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->multiplyAllBySA();
    non_adaptive_out[i]->multiply(ds.solidAngle(ds.nonAdapIndex(i)));
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) *= ds.solidAngle(iter.outIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::multiplyAllByInSACT()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding in-direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::multiplyAllByInSACT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->multiply(ds.solidAngleAbsCosTheta(ds.nonAdapIndex(i)));
    non_adaptive_out[i]->multiplyAllBySACT();
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) *= ds.solidAngleAbsCosTheta(iter.inIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::multiplyAllByOutSACT()
// this requires (type A) *= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding out direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::multiplyAllByOutSACT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->multiplyAllBySACT();
    non_adaptive_out[i]->multiply(ds.solidAngleAbsCosTheta(ds.nonAdapIndex(i)));
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) *= ds.solidAngleAbsCosTheta(iter.outIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByInSA()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding in-direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByInSA() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(ds.solidAngle(ds.nonAdapIndex(i)));
    non_adaptive_out[i]->divideAllBySA();
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= ds.solidAngle(iter.inIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByOutSA()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding out direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByOutSA() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divideAllBySA();
    non_adaptive_out[i]->divide(ds.solidAngle(ds.nonAdapIndex(i)));
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= ds.solidAngle(iter.outIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByInSACT()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding in-direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByInSACT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(ds.solidAngleAbsCosTheta(ds.nonAdapIndex(i)));
    non_adaptive_out[i]->divideAllBySACT();
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= ds.solidAngleAbsCosTheta(iter.inIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByOutSACT()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle times abs cos theta of the corresponding out direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByOutSACT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divideAllBySACT();
    non_adaptive_out[i]->divide(ds.solidAngleAbsCosTheta(ds.nonAdapIndex(i)));
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= ds.solidAngleAbsCosTheta(iter.outIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByInCT()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the solid angle of the corresponding in-direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByInCT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(fabs(ds.meanCosTheta(ds.nonAdapIndex(i))));
    non_adaptive_out[i]->divideAllByCT();
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= fabs(ds.meanCosTheta(iter.inIndex())); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAllByOutCT()
// this requires (type A) /= (double) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf by the abs cos theta of the corresponding out direction quad

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::divideAllByOutCT() {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divideAllByCT();
    non_adaptive_out[i]->divide(fabs(ds.meanCosTheta(ds.nonAdapIndex(i))));
  }

    // walk the leaf nodes dividing by solid angle
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= fabs(ds.meanCosTheta(iter.outIndex())); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAtInIndices()
// this requires (type A) /= (type F) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf the corresponding data of (dt) matched to the in direction quad

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::divideAtInIndices(const DirecTable<F,DS>& dt) {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(dt.atIndex(ds.nonAdapIndex(i)));
    non_adaptive_out[i]->divide(dt);
  }

    // walk the leaf nodes dividing by the relevant entry for in index
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= dt.atIndex(iter.inIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecSparse::divideAtOutIndices()
// this requires (type A) /= (type F) to be defined
// doesn't do anything with base_data so makes the most sense when base_data is zero(s)
// divides the data at each leaf the corresponding data of (dt) matched to the out-direction quad

template <typename A, typename DS>
template <typename F>
void DirecDirecSparse<A,DS>::divideAtOutIndices(const DirecTable<F,DS>& dt) {

    // deal with the non-adaptive tables
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->divide(dt);
    non_adaptive_out[i]->divide(dt.atIndex(ds.nonAdapIndex(i)));
  }

    // walk the leaf nodes dividing by the relevant entry for in index
  DDS_LeafIter<A,DS> iter(*this);
  A* to = iter.first();
  while (to) {
    (*to) /= dt.atIndex(iter.outIndex()); 
    to = iter.next();
  }

  lazy_valid = false;
}



// DirecDirecTable::sumOfBackwardEntries()

template <typename A, typename DS> 
const A DirecDirecSparse<A,DS>::sumOfBackwardEntries() const {

  A sum(0);

  return sum;
}



// DirecDirecSparse::read()

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::read(JDataStream& str) {

  str >> base_data;
  base_data_is_null = (base_data == A(0));

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->read(str);
    non_adaptive_out[i]->read(str);
  }

  for (int i=0;i<ds.adapTopNodesSQ();i++) {
    bool flag;
    str >> flag;
    if (flag) {
      top_node[i] = new DDS_Node<A>();
      top_node[i]->recursiveRead(str, ds.adapMaxDepth()-1);
    } else top_node[i] = 0;
  }

  lazy_valid = false;
}



// DirecDirecSparse::write()

template <typename A, typename DS>
void DirecDirecSparse<A,DS>::write(JDataStream& str) const {

  str << base_data;

  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    non_adaptive_in[i]->write(str);
    non_adaptive_out[i]->write(str);
  }

  for (int i=0;i<ds.adapTopNodesSQ();i++) {
    str << (bool) (top_node[i] != 0); 
    if (top_node[i]) top_node[i]->recursiveWrite(str, ds.adapMaxDepth()-1);
  }
}



#endif // _DIRECDIRECSPARSE_H_JDH

