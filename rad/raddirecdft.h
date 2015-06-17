
// raddirecdft.h

#ifndef _JUDE_RAD_RADDIRECDFT_H
#define _JUDE_RAD_RADDIRECDFT_H

#include "rad/rd_matrixdft.h"
#include "math/ublas.h"

using namespace jude::math;

namespace jude {
namespace rad {



template <typename SD, typename DS> 
class RadDirecDFT {

  DS ds;
  SD* data;  

  int band_count;
 
  bool lazy_valid;

  SD zero_sd; // not sure if this is really necessary

  int pos1(int row, int lm) const { 
    if (row==0) return (lm==0) ? 0 : -1;
    if (ds.isFull() && row==ds.rowsDFT()-1) return (lm==0) ? ds.units()-1 : -1; 
    return 1 + (row-1)*ds.horizSize() + lm;
  } 

  int pos2(int row, int lm) const { 
    if (lm==0 || lm==ds.lmNumDFT()-1 || row==0 || (ds.isFull() && row==ds.rowsDFT()-1)) return -1;
    return 1 + (row-1)*ds.horizSize() + ds.horizSize()/2 + lm;
  } 

    // we assume these non-const private versions are only called with valid (row) and (lm) values
  SD& f1_unsafe(int row, int lm) { return data[pos1(row,lm)]; }
  SD& f2_unsafe(int row, int lm) { return data[pos2(row,lm)]; }

public:

  RadDirecDFT<SD,DS>(const DS& ds, int bc) : 
    ds(ds), 
    data(new SD[ds.units()]),
    band_count(bc),
    lazy_valid(false),
    zero_sd(band_count, 0) { }

  RadDirecDFT<SD,DS>(const RadDirecImp<SD>& si) :
    ds(si.direcStruct()), 
    data(new SD[ds.units()]),
    band_count(si.bandCount()),   
    lazy_valid(false),
    zero_sd(band_count, 0) { copy(si); }

  ~RadDirecDFT() {
    delete[] data;
  }

  const DS& direcStruct() const { return ds; }

    /*! \brief Returns the number of spectral bands of the data.*/
  int bandCount() const { return band_count; } 

  void makeZero() { for (int i=0; i<ds.units(); i++) data[i] = SD(band_count,0); }

  void copy(const RadDirecImp<SD>& si);

  void copyWholeMatrices(const matrix<typename SD::elem_type>* w1, 
			 const matrix<typename SD::elem_type>* w2);

  void buildWholeMatrices(matrix<typename SD::elem_type>* w1, 
			  matrix<typename SD::elem_type>* w2) const;

  void copyFromMatrixDFT(const RD_MatrixDFT& sm);
  void copyToMatrixDFT(RD_MatrixDFT& sm) const;

  const SD& f1(int row, int lm) const { int p = pos1(row,lm); return (p<0) ? zero_sd : data[p]; }
  const SD& f2(int row, int lm) const { int p = pos2(row,lm); return (p<0) ? zero_sd : data[p]; }

  const SD atIndex(int i) const;

  void display(JTextStream& str) const;
};



// RadDirecDFT::copy()

template <typename SD, typename DS> 
void RadDirecDFT<SD,DS>::copy(const RadDirecImp<SD>& si) {

  int n = ds.lmNumDFT()-1;
  int index_base = 1;

    // for each horizontal row (note: deal with end caps separately later)
  for (int row=1; row<=ds.vertSize(); row++, index_base+=ds.horizSize()) {
  
      // go across the the row
    for (int lm=0; lm<=n; lm++) {

      SD f1_sum(band_count, 0);
      SD f2_sum(band_count, 0);

        // sum the cos and sin factors
      for (int v=0; v<ds.horizSize(); v++) {
	int index = index_base + v;
	double phi_v = (v*2*M_PI) / ds.horizSize();
	double lm_phi_v = lm * phi_v;
	const SD fv = si.atIndex(index);
	f1_sum += fv * cos(lm_phi_v);
	f2_sum += fv * sin(lm_phi_v);
      }

      f1_sum /= ((lm==0 || lm==n) ? ds.horizSize() : n);
      f2_sum /= n;
      
      f1_unsafe(row, lm) = f1_sum;
      if (lm>0 && lm<n) f2_unsafe(row, lm) = f2_sum;
    }
  }

    // end caps
  data[0] = si.atIndex(0);
  if (ds.isFull()) data[ds.units()-1] = si.atIndex(ds.units()-1);
}



// RadDirecDFT::copy()
// copy from from whole-matrix representation
// wm1 is band sized array of 1 x ds.rowsDFT()*ds.lmNumDFT() matrices of f1 values
// wm2 is band sized array of 1 x ds.rowsDFT()*ds.lmNumDFT() matrices of f2 values

template <typename SD, typename DS> 
void RadDirecDFT<SD,DS>::copyWholeMatrices(const matrix<typename SD::elem_type>* w1, 
					   const matrix<typename SD::elem_type>* w2) {
 
  int p = 0;
  for (int lm=0; lm<ds.lmNumDFT(); lm++) {
    for (int row=0; row<ds.rowsDFT(); row++, p++) {
 
      SD sd1(band_count);
      SD sd2(band_count);

      for (int band=0; band<band_count; band++) {
	sd1[band] = w1[band](0,p);
	sd2[band] = w2[band](0,p);
      }

      int f1_pos = pos1(row, lm);
      int f2_pos = pos2(row, lm);
   
      if (f1_pos >= 0) data[f1_pos] = sd1;
      if (f2_pos >= 0) data[f2_pos] = sd2;  
    }
  }
}



// SpecDFT::buildWholeMatrices() 
// converts the SpecDFT into and array of matrices each sized 1 x (ds.rows*ds.lmNum), with one matrix for each band
// the factors for each l-mode are one after another in the matrix (see L&W pg. 418)
// this matrix format is suitable for post mutiplication by the "wholeMatrix" reflection types returned 
// from ReflecDFT, ReflecDFTSym, PlaneSlab etc. to give reflected radiance distribution in each band

template <typename SD, typename DS> 
  void RadDirecDFT<SD,DS>::buildWholeMatrices(matrix<typename SD::elem_type>* w1, 
					      matrix<typename SD::elem_type>* w2) const {

  for (int band=0; band<bandCount(); band++) {
    w1[band].resize(1, ds.rowsDFT()*ds.lmNumDFT());
    w2[band].resize(1, ds.rowsDFT()*ds.lmNumDFT());
  }

  int p = 0;
  for (int lm=0; lm<ds.lmNumDFT(); lm++) {
    for (int row=0; row<ds.rowsDFT(); row++, p++) {
 
      const SD& sd1 = f1(row, lm);
      const SD& sd2 = f2(row, lm);

      for (int band=0; band<bandCount(); band++) {
	w1[band](0,p) = sd1.bandValue(band);
	w2[band](0,p) = sd2.bandValue(band);
      }
    }
  }
}



// RadDirecDFT::copyFromMatrixDFT()
// should check directional structure of sm also

template <typename SD, typename DS> 
void RadDirecDFT<SD,DS>::copyFromMatrixDFT(const RD_MatrixDFT& sm) {
    // check band counts are the same
  if (sm.bandCount() != bandCount()) { jlog::es << "RadDirecDFT::copyMatrixArray() band counts differ"; internalError(); }
    // copy in data from matrix arrays
  copyWholeMatrices(sm.array1(), sm.array2()); 
}



// SpecDFT::convertToMatrixArray()

template <typename SD, typename DS> 
void RadDirecDFT<SD,DS>::copyToMatrixDFT(RD_MatrixDFT& sm) const {
    // set up the number of bands
  sm.setup(bandCount());
    // create the matrices (the sizes of which depend on directional structure)
  buildWholeMatrices(sm.array1(), sm.array2()); 
}



// RadDirecDFT::atIndex()

template <typename SD, typename DS> 
const SD RadDirecDFT<SD,DS>::atIndex(int index) const {

    // end caps
  if (index==0) return data[0];
  if (ds.isFull() && index==ds.units()-1) return data[ds.units()-1];

    // the row of this index
  int row = ((index-1) / ds.horizSize()) + 1; 
 
    // start of cos and sin data
  SD* ptr = data + 1 + (row-1) * ds.horizSize();

  SD fv(band_count,0);
  int n = ds.horizSize() / 2;
	
    // phi value of this quad
  double phi_v = ((index - 1 - (row-1)* ds.horizSize()) * 2*M_PI) / ds.horizSize();
    // sum the cos and sin factors
  for (int lm=0; lm<=n; lm++,ptr++) fv += (*ptr) * cos(lm*phi_v);
  for (int lm=1; lm<n; lm++,ptr++) fv += (*ptr) * sin(lm*phi_v);
 
  return fv;
}



// RadDirecDFT::display()

template <typename SD, typename DS> 
void RadDirecDFT<SD,DS>::display(JTextStream& str) const {
  jlog::ls << "band_count: " << band_count << "\n";
  for (int i=0; i<ds.units(); i++) {
    str << i << ": " << atIndex(i) << "\n"; 
  }

  //int index_base = 1;
  //for (int i=0; i<ds.vertSize(); i++,index_base+=ds.horizSize()) {
  ////for (int j=0; j<=ds.horizSize()/2; j++) f1(i,j).display();
  // //for (int j=1; j<ds.horizSize()/2; j++) f2(i,j).display();
  //for (int j=0;j<ds.horizSize();j++) data[index_base + j].display();
  //}
}



} } // namespace jude::rad

#endif // _JUDE_RADDIRECDFT_H_JDH


