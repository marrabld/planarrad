
// rdd_matrixdft.cc

#include "rad/rdd_matrixdft.h"
#include "rad/rd_matrixdft.h"
#include "base/jlog.h"
#include "base/jdatastream.h"

using namespace jude::rad;
using namespace jude::base;



// RDD_MatrixDFT::(constructor)

RDD_MatrixDFT::RDD_MatrixDFT() :
  band_count(0),
  lm_count(0),
  row_count(0),
  w11(0),
  w12(0),
  w21(0),
  w22(0) { }



// RDD_MatrixDFT::(constructor)

RDD_MatrixDFT::RDD_MatrixDFT(const RDD_MatrixDFT& rm) :
  w11(0),
  w12(0),
  w21(0),
  w22(0) { 

  copy(rm);
}



// RDD_MatrixDFT::copy()

void RDD_MatrixDFT::copy(const RDD_MatrixDFT& rm) {

  clear();

  band_count = rm.band_count;
  lm_count = rm.lm_count;
  row_count = rm.row_count;

  if (rm.w11) {
    w11 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) w11[i] = rm.w11[i];
  }

  if (rm.w12) {
    w12 = new matrix<double>[band_count];
    w21 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) {
      w12[i] = rm.w12[i];
      w21[i] = rm.w21[i];
    }
  }

  if (rm.w22) {
    w22 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) w22[i] = rm.w22[i];
  }
}



// RDD_MatrixDFT::setup1()

void RDD_MatrixDFT::setup1(int bc, int lc, int rc) {
  clear();
  w11 = new matrix<double>[bc];
  band_count = bc;
  lm_count = lc;
  row_count = rc;
}



// RDD_MatrixDFT::setup2()

void RDD_MatrixDFT::setup2(int bc, int lc, int rc) {
  clear();
  w11 = new matrix<double>[bc];
  w22 = new matrix<double>[bc];
  band_count = bc;
  lm_count = lc;
  row_count = rc;
}



// RDD_MatrixDFT::setup4()

void RDD_MatrixDFT::setup4(int bc, int lc, int rc) {
  clear();
  w11 = new matrix<double>[bc];
  w12 = new matrix<double>[bc];
  w21 = new matrix<double>[bc];
  w22 = new matrix<double>[bc];
  band_count = bc;
  lm_count = lc;
  row_count = rc;
}



// RDD_MatrixDFT::clear()

void RDD_MatrixDFT::clear() {
  delete[] w11;
  delete[] w12;
  delete[] w21;
  delete[] w22;
  band_count = 0;
  lm_count = 0;
  row_count = 0;
  w11 = 0;
  w12 = 0;
  w21 = 0;
  w22 = 0;
}



// RDD_MatrixDFT::translateInToOut()

void RDD_MatrixDFT::translateInToOut(const RD_MatrixDFT& sm, RD_MatrixDFT& res) const {
  
  assert(band_count==1 || sm.bandCount() == band_count);

    // allocate matrices for each band in result structure
  res.setup(sm.bandCount());

  int func_band = 0;

    // if w22 is undefined then only one matrix array (w11) is defined
    // this occurs for slab transmission and bare slab reflectances 
  if (isSetup1()) {

    for (int band=0; band<sm.bandCount(); band++) {

      res.array1()[band].resize(1, lm_count * row_count);
      res.array2()[band].resize(1, lm_count * row_count);

      //res.array1()[band] = prod(sm.array1()[band], w11[func_band]);
      //res.array2()[band] = prod(sm.array2()[band], w11[func_band]);

      for (int lm=0; lm<lm_count; lm++) {
	range r(lm*row_count, (lm+1)*row_count);
	project(res.array1()[band], range(0,1), r) = prod(project(sm.array1()[band], range(0,1), r), project(w11[func_band],r,r));
      	project(res.array2()[band], range(0,1), r) = prod(project(sm.array2()[band], range(0,1), r), project(w11[func_band],r,r));
      }

      if (band_count!=1) func_band++;
    }
 
    // if w12 and w21 are undefined then only need to do half the multiplications
    // this is like what occurs with slab reflectance integrated to include substrate relectance where w12=w21=0
  } else if (w12 == 0) {

    for (int band=0; band<sm.bandCount(); band++) {
      res.array1()[band] = prod(sm.array1()[band], w11[func_band]);
      res.array2()[band] = prod(sm.array2()[band], w22[func_band]);
      if (band_count!=1) func_band++;
    }

    // all matrices defined
  } else {

    for (int band=0; band<sm.bandCount(); band++) {
      res.array1()[band] = prod(sm.array1()[band], w11[func_band]) + prod(sm.array2()[band], w21[func_band]);
      res.array2()[band] = prod(sm.array1()[band], w12[func_band]) + prod(sm.array2()[band], w22[func_band]);
      if (band_count!=1) func_band++;
    }
  }
}



// RDD_MatrixDFT::postMultiply()
// makes (*this) the result of (*this) followed by (rm)

void RDD_MatrixDFT::postMultiply(const RDD_MatrixDFT& rm) {
  
  assert(rm.bandCount() == band_count);

    // setup1 is block diagonal with only w11 used
    // this occurs for slab transmission and bare slab reflectances 
  if (isSetup1()) {

    if (rm.isSetup1()) {
      for (int band=0; band<band_count; band++) {
	for (int lm=0; lm<lm_count; lm++) {
	  range r(lm*row_count, (lm+1)*row_count);
	  project(w11[band],r,r) = prod(project(w11[band],r,r), project(rm.w11[band],r,r));
	}
      }
    } else if (rm.isSetup2()) {
      w22 = new matrix<double>[band_count];
      for (int band=0; band<band_count; band++) {
	w11[band] = prod(w11[band], rm.w11[band]);
	w22[band] = prod(w11[band], rm.w22[band]);
      }
    } else {
      // todo
    }
     
    // if w12 and w21 are undefined then only need to do half the multiplications
    // this is like what occurs with slab reflectance integrated to include substrate relectance where w12=w21=0
  } else if (isSetup2()) {

    if (rm.isSetup1()) {
      for (int band=0; band<band_count; band++) {
	w11[band] = prod(w11[band], rm.w11[band]);
	w22[band] = prod(w22[band], rm.w11[band]);
      }
    } else if (rm.isSetup2()) {
      for (int band=0; band<band_count; band++) {
	w11[band] = prod(w11[band], rm.w11[band]);
	w22[band] = prod(w22[band], rm.w22[band]);
      }
    } else {
      // todo
    }

    // all matrices defined
  } else {

    // todo
  }
}



// RDD_MatrixDFT::postMultiply()
// makes (*this) the result of (*this) followed by (rm)

void RDD_MatrixDFT::add(const RDD_MatrixDFT& rm) {
  
  assert(rm.bandCount() == band_count);

    // setup1 is block diagonal with only w11 used
    // this occurs for slab transmission and bare slab reflectances 
  if (isSetup1()) {

    if (rm.isSetup1()) {
      for (int band=0; band<band_count; band++) {
	for (int lm=0; lm<lm_count; lm++) {
	  range r(lm*row_count, (lm+1)*row_count);
	  project(w11[band],r,r) += project(rm.w11[band],r,r);
	}
      }
    } else if (rm.isSetup2()) {
      w22 = new matrix<double>[band_count];
      for (int band=0; band<band_count; band++) {
	w11[band] += rm.w11[band];
	w22[band] = rm.w22[band];
      }
    } else {
      // to do
    }

    // if w12 and w21 are undefined then only need to do half the multiplications
    // this is like what occurs with slab reflectance integrated to include substrate relectance where w12=w21=0
  } else if (isSetup2()) {
 
    if (rm.isSetup1()) {
      for (int band=0; band<band_count; band++) {
	w11[band] += rm.w11[band];
	w22[band] += rm.w11[band];
      }
    } else if (rm.isSetup2()) {
      for (int band=0; band<band_count; band++) {
	w11[band] += rm.w11[band];
	w22[band] += rm.w22[band];
      }
    } else {
      // to do
    }

    // all matrices defined
  } else {

    for (int band=0; band<band_count; band++) {
      w11[band] += rm.w11[band];
      w12[band] += rm.w12[band];
      w21[band] += rm.w21[band];
      w22[band] += rm.w22[band];
    }
  }
}



// RDD_MatrixDFT::makeIdentityMinusInverse()

bool RDD_MatrixDFT::makeIdentityMinusInverse() const {
  
  matrix<double> temp;
  matrix<double> temp2;
    
    // setup1 is block diagonal with only w11 used
    // this occurs for slab transmission and bare slab reflectances 
  if (isSetup1()) {
      
    for (int band=0; band<band_count; band++) {
      
      for (int lm=0; lm<lm_count; lm++) {
	range r(lm*row_count, (lm+1)*row_count);
	matrix<double> temp(identity_matrix<double>(row_count) - project(w11[band],r,r)); 
	if (!invert(temp, temp2)) return false;
	project(w11[band],r,r) = temp2;
      }      
    }

    // if w12 and w21 are undefined then only need to do half the multiplications
    // this is like what occurs with slab reflectance integrated to include substrate relectance where w12=w21=0
  } else if (isSetup2()) {

    for (int band=0; band<band_count; band++) {
      matrix<double> temp(identity_matrix<double>(w11[band].size1()) - w11[band]); 
      if (!invert(temp, w11[band])) return false;
      temp = identity_matrix<double>(w22[band].size1()) - w22[band]; 
      if (!invert(temp, w22[band])) return false;
    }

    // all matrices defined
  } else {

    for (int band=0; band<band_count; band++) {
      matrix<double> temp(identity_matrix<double>(w11[band].size1()) - w11[band]); 
      if (!invert(temp, w11[band])) return false;
      temp = identity_matrix<double>(w12[band].size1()) - w12[band]; 
      if (!invert(temp, w12[band])) return false;
      temp = identity_matrix<double>(w21[band].size1()) - w21[band]; 
      if (!invert(temp, w21[band])) return false;
      temp = identity_matrix<double>(w22[band].size1()) - w22[band]; 
      if (!invert(temp, w22[band])) return false;
    }
  }

  return true;
}



// RDD_MatrixDFT::writeToDataStream()

void RDD_MatrixDFT::writeToDataStream(JDataStream& str) const {

  str << band_count;
  str << lm_count;
  str << row_count;
  
  if (isSetup1()) {

      // write setup code
    str << ((unsigned char) 1);
      // only need to write blocks on diagonal
    for (int i=0; i<band_count; i++) {
      for (int lm=0; lm<lm_count; lm++) {
	range r(lm*row_count, (lm+1)*row_count);
	matrix<double> temp = project(w11[i], r, r); 
	writeMatrix(temp, str);
      }
    }

  } else {

      // write setup code
    str << ((unsigned char) 4);

    str << (w11 != 0);
    if (w11) for (int i=0; i<band_count; i++) writeMatrix(w11[i], str);
    
    str << (w12 != 0);
    if (w12) for (int i=0; i<band_count; i++) writeMatrix(w12[i], str);
    
    str << (w21 != 0);
    if (w21) for (int i=0; i<band_count; i++) writeMatrix(w21[i], str);
    
    str << (w22 != 0);
    if (w22) for (int i=0; i<band_count; i++) writeMatrix(w22[i], str);
  }
}



// RDD_MatrixDFT::readFromDataStream()
// FIXME

void RDD_MatrixDFT::readFromDataStream(JDataStream& str) {

  clear();

  str >> band_count;
  str >> lm_count;
  str >> row_count;

    // get setup code
  unsigned char s;
  str >> s;

  matrix<double> temp;

  if (s == 1) {

    w11 = new matrix<double>[band_count];

    for (int i=0; i<band_count; i++) {
      w11[i] = zero_matrix<double>(lm_count*row_count, lm_count*row_count);
      for (int lm=0; lm<lm_count; lm++) {
	readMatrix(temp, str);
	range r(lm*row_count, (lm+1)*row_count);
	project(w11[i],r,r) = temp;
      }
    }

  } else {

  bool f;

  str >> f;
  if (f) {
    w11 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) readMatrix(w11[i], str);
  }

  str >> f;
  if (f) {
    w12 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) readMatrix(w12[i], str);
  }

  str >> f;
  if (f) {
    w21 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) readMatrix(w21[i], str);
  }

  str >> f;
  if (f) {
    w22 = new matrix<double>[band_count];
    for (int i=0; i<band_count; i++) readMatrix(w22[i], str);
  }
  }
}



// RDD_MatrixDFT::testIsBlockDiagonal()

bool RDD_MatrixDFT::testIsBlockDiagonal() const {

  for (int band=0; band<band_count; band++) {

    for (int lm1=0; lm1<lm_count; lm1++) {

      int offset1 = lm1*row_count;
      
      for (int lm2=0; lm2<lm_count; lm2++) {
	
	if (lm1 == lm2) continue;
	
	int offset2 = lm2*row_count;
	
	for (int i=0; i<row_count; i++) for (int j=0; j<row_count; j++) if (w11[band](offset1+i, offset2+j) != 0) return false;
      }
    }
  }

  return true;
}



// RDD_MatrixDFT::display()

void RDD_MatrixDFT::display(JTextStream& str) const {
 
  str << "RDD_MatrixDFT\n";
  str << "band_count: " << band_count << "\n";
  str << "lm_count: " << lm_count << "\n";
  str << "row_count: " << row_count << "\n";

  for (int i=0; i<band_count; i++) {

    str << "band " << i << ":\n";

    str << "w11:\n";
    if (w11 != 0) str << w11[i];
    else str << "(null)\n";

    str << "w12:\n";
    if (w12 != 0) str << w12[i];
    else str << "(null)\n";

    str << "w21:\n";
    if (w21 != 0) str << w21[i];
    else str << "(null)\n";

    str << "w22:\n";
    if (w22 != 0) str << w22[i];
    else str << "(null)\n";
  }
}

