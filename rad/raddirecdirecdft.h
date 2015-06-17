
// raddirecdirecdft.h

#ifndef _JUDE_RAD_RADDIRECDIRECDFT_H
#define _JUDE_RAD_RADDIRECDIRECDFT_H

#include "rad/rdd_matrixdft.h"
#include "math/ublas.h"

using namespace jude::math;

namespace jude {
namespace rad {



template <typename SD, typename DS> 
class RadDirecDirecDFT {

  DS ds;
  SD* data;  

  int band_count;

  bool lazy_valid;

  int rowSelect(int in_row, int out_row) const { return ((in_row * ds.rowsDFT()) + out_row) * ds.lmNumDFT() * ds.lmNumDFT(); }

  int units() const { return 4 * (ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT() * ds.lmNumDFT()); }

  int h11Base() const { return 0; }
  int h12Base() const { return ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT() * ds.lmNumDFT(); }
  int h21Base() const { return 2 * ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT() * ds.lmNumDFT(); }
  int h22Base() const { return 3 * ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT() * ds.lmNumDFT(); }

public:

  RadDirecDirecDFT<SD,DS>(const DS& ds, int bc) : 
    ds(ds), 
    data(new SD[units()]), 
    band_count(bc),
    lazy_valid(false) { }

  RadDirecDirecDFT<SD,DS>(const RadDirecDirecImp<SD>& ri, double* pct=0, bool* cancel=0) :
    ds(ri.direcStruct()), 
    data(new SD[units()]),
    band_count(ri.bandCount()),
    lazy_valid(false) { copy(ri, pct); }

  ~RadDirecDirecDFT() {
    delete[] data;
  }

  const DS& direcStruct() const { return ds; }

    /*! \brief Returns the number of spectral bands of the data.*/
  int bandCount() const { return band_count; } 

  void makeZero() { for (int i=0; i<units(); i++) data[i] = SD(band_count,0); }

  void copy(const RadDirecDirecImp<SD>& ri, double* pct=0, bool* cancel=0);

  SD& h11(int in_row, int out_row, int km, int lm) { return *(data + h11Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  SD& h12(int in_row, int out_row, int km, int lm) { return *(data + h12Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  SD& h21(int in_row, int out_row, int km, int lm) { return *(data + h21Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  SD& h22(int in_row, int out_row, int km, int lm) { return *(data + h22Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }

  const SD& h11(int in_row, int out_row, int km, int lm) const { return *(data + h11Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  const SD& h12(int in_row, int out_row, int km, int lm) const { return *(data + h12Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  const SD& h21(int in_row, int out_row, int km, int lm) const { return *(data + h21Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }
  const SD& h22(int in_row, int out_row, int km, int lm) const { return *(data + h22Base() + rowSelect(in_row, out_row) + km*ds.lmNumDFT() + lm); }

  const SD atIndices(int in_index, int out_index) const;

  void copyWholeMatrices(const matrix<double>* w11, const matrix<double>* w12, const matrix<double>* w21, const matrix<double>* w22);
  void buildWholeMatrices(matrix<double>* w11, matrix<double>* w12, matrix<double>* w21, matrix<double>* w22) const;

  void buildIndividualMatrices(matrix<double>* w11, matrix<double>* w12, matrix<double>* w21, matrix<double>* w22) const;

  void copyFromMatrixDFT(const RDD_MatrixDFT& rm);
  void copyToMatrixDFT(RDD_MatrixDFT& rm) const;

  const SD diffuseReflectance() const;

  void display(JTextStream& str) const;
};



// ReflecDFT::copy()
// this operation is quite slow
// maybe fast fourier transform can be used ?

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::copy(const RadDirecDirecImp<SD>& ri, double* pct, bool* cancel) {

  double div = 100.0 / (ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT() * ds.lmNumDFT());
  int count = 0;

    // for each in-row
  for (int r=0; r<ds.rowsDFT(); r++) {
      // for each out-row
    for (int u=0; u<ds.rowsDFT(); u++) {

        // for each l-mode
      for (int lm=0; lm<ds.lmNumDFT(); lm++) {    
	  // for each k-mode
	for (int km=0; km<ds.lmNumDFT(); km++) {

	  SD h11_sum(band_count, 0);
	  SD h12_sum(band_count, 0);
	  SD h21_sum(band_count, 0);
	  SD h22_sum(band_count, 0);
      
	    // for each in-position
	  for (int s=0; s<ds.horizSize(); s++) {

	    double km_phi_s = km * (s * 2*M_PI) / ds.horizSize();
	
	      // for each out-position
	    for (int v=0; v<ds.horizSize(); v++) {
 
	      double lm_phi_v = lm * (v * 2*M_PI) / ds.horizSize();

	      int in_index = ds.positionToIndexDFT(r,s);
	      int out_index = ds.positionToIndexDFT(u,v);
	      
	      SD hsv = ri.atIndices(in_index, out_index);
	      
	      h11_sum += hsv * cos(km_phi_s) * cos(lm_phi_v);
	      h12_sum += hsv * cos(km_phi_s) * sin(lm_phi_v);
	      h21_sum += hsv * sin(km_phi_s) * cos(lm_phi_v);
	      h22_sum += hsv * sin(km_phi_s) * sin(lm_phi_v);
	    }
	  }

	  h11_sum /= ds.epsilonDFT(km) * ds.epsilonDFT(lm);
	  if (ds.gammaDFT(lm)) h12_sum /= ds.epsilonDFT(km) * ds.gammaDFT(lm);
	  if (ds.gammaDFT(km)) h21_sum /= ds.gammaDFT(km) * ds.epsilonDFT(lm);
	  if (ds.gammaDFT(lm) && ds.gammaDFT(km)) h22_sum /= ds.gammaDFT(km) * ds.gammaDFT(lm);
	  else h22_sum = SD(band_count, 0);
	  
	  h11(r, u, km, lm) = h11_sum;
	  h12(r, u, km, lm) = h12_sum;
	  h21(r, u, km, lm) = h21_sum;
	  h22(r, u, km, lm) = h22_sum;

	  if (pct) (*pct) = count * div;
	  //jlog::ls << "pct " << (*pct) << "\n";
	  count++;
	  if (cancel) if (*cancel) return;

	  /*
	  if (lm==0 || (ds.isFull() && lm==ds.lmNumDFT()-1)) h12(r,u,km,0) = 0;
	  if (km==0 || (ds.isFull() && km==ds.lmNumDFT()-1)) h21(r,u,0,lm) = 0;
	  if ((lm==0 && km==0) || (ds.isFull() && lm==ds.lmNumDFT()-1 && km==ds.lmNumDFT()-1)) h22(r,u,km,lm) = 0;
	  if (ds.isFull()) {
	    if ((lm==0 && km==ds.lmNumDFT()-1) || (km==0 && lm==ds.lmNumDFT()-1)) h22(r,u,km,lm) = 0;
	  }
	  */
	}
      }
    }
  }

  if (pct) (*pct) = 100;
}



// RadDirecDirecDFT::atIndices()

template <typename SD, typename DS> 
const SD RadDirecDirecDFT<SD,DS>::atIndices(int in_index, int out_index) const {

  int n = ds.horizSize() / 2;

  int r; // in_row
  int s; // in_column
  int u; // out_row
  int v; // out_column
 
  ds.indexToPositionDFT(in_index, r, s);
  ds.indexToPositionDFT(out_index, u, v);

    // angular positions
  double phi_s = (s * 2*M_PI) / ds.horizSize();
  double phi_v = (v * 2*M_PI) / ds.horizSize();

     // sum over each l-mode
  SD sum(band_count, 0);
  
  for (int km=0; km<=n; km++) {
    for (int lm=0; lm<=n; lm++) {
      sum += h11(r, u, km, lm) * cos(km * phi_s) * cos(lm * phi_v);
      sum += h12(r, u, km, lm) * cos(km * phi_s) * sin(lm * phi_v);
      sum += h21(r, u, km, lm) * sin(km * phi_s) * cos(lm * phi_v);
      sum += h22(r, u, km, lm) * sin(km * phi_s) * sin(lm * phi_v);
    }
  }
  
  return sum;
}



// RadDirecDirecDFT::buildIndividualMatrices() 
// builds an array of mxm matrices (m = ds.rows) one for each band and each l-mode and each k-mode
// in general this does not decouple in cos and sin factors and gives four matrices
// m11 (cos->cos), m12 (cos->sin ??), m21 (sin->cos ??), m22 (sin->sin)

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::buildIndividualMatrices(matrix<double>* m11, matrix<double>* m12, matrix<double>* m21, matrix<double>* m22) const {

  for (int i=0; i<band_count*ds.lmNumDFT()*ds.lmNumDFT(); i++) {
    m11[i].resize(ds.rowsDFT(), ds.rowsDFT());
    m12[i].resize(ds.rowsDFT(), ds.rowsDFT());
    m21[i].resize(ds.rowsDFT(), ds.rowsDFT());
    m22[i].resize(ds.rowsDFT(), ds.rowsDFT());
  }

  // not implemented
}



template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::copyWholeMatrices(const matrix<double>* w11, const matrix<double>* w12, const matrix<double>* w21, const matrix<double>* w22) {

  jlog::ls.printf("RadDirecDirecDFT::copyWholeMatrices() - check me\n");

  makeZero();
  
  for (int km=0; km<ds.lmNumDFT(); km++) {
    int i_offset = km*ds.rowsDFT();
    
    for (int lm=0; lm<ds.lmNumDFT(); lm++) {
      int j_offset = lm*ds.rowsDFT();
      
      SD sd11(band_count,0);
      SD sd12(band_count,0);
      SD sd21(band_count,0);
      SD sd22(band_count,0);

        // for each in-direction horizontal row
      for (int r=1; r<=ds.vertSize(); r++) {
	  // for each out-direction horizontal row
	for (int u=0; u<=ds.vertSize(); u++) {
	  
	  for (int band=0; band<band_count; band++) {

	    sd11[band] = w11[band](i_offset+r, j_offset+u) / ds.epsilonDFT(km);
	    if (w12) sd12[band] = w12[band](i_offset+r, j_offset+u) / ds.epsilonDFT(km);
	    if (ds.gammaDFT(km)) {
	      if (w21) sd21[band] = w21[band](i_offset+r, j_offset+u) / ds.gammaDFT(km);
	      if (w22) sd22[band] = w22[band](i_offset+r, j_offset+u) / ds.gammaDFT(km);
	    }
	  }
	  
	  h11(r,u,km,lm) = sd11;
	  h12(r,u,km,lm) = sd12;
	  h21(r,u,km,lm) = sd21;
	  h22(r,u,km,lm) = sd22;
	}
      }

          // into end cap (r==0) is special case cos factor goes to all quads equally (i.e only km=0 is defined)
          // for each out-direction horizontal row
         // TODO check that the other vlaues are really 0 = might be duplicates

      for (int u=0; u<=ds.vertSize(); u++) {
	for (int band=0; band<band_count; band++) {
	  h11(0,u,0,lm)[band] = w11[band](i_offset, j_offset+u);
	}
      }
    }
  }
}



// ReflecDFT::buildWholeMatrices() 
// builds four matrices for each band each of which which contain the mxm matrices for all l-modes and k-modes
// the four matrices are required because in general ReflecDFT does not decouple wrt to cos and sin
// m11 (cos->cos), m12 (cos->sin ??), m21 (sin->cos ??), m22 (sin->sin) the layout is as in L&W pg. 418
// this format is the same as returned by "wholeMatrix" types in ReflecDFTSym and PlaneSlab
// suitable for multiplying SpecDFT wholeMatrices by to give reflected or transmitted radiance i.e. Lr = L * R

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::buildWholeMatrices(matrix<double>* w11, matrix<double>* w12, matrix<double>* w21, matrix<double>* w22) const {

  for (int band=0; band<band_count; band++) {

    w11[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
    w12[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
    w21[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
    w22[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
  
    for (int km=0; km<ds.lmNumDFT(); km++) {
      int i_offset = km*ds.rowsDFT();

      for (int lm=0; lm<ds.lmNumDFT(); lm++) {
	int j_offset = lm*ds.rowsDFT();

          // for each in-direction horizontal row
	for (int r=1; r<=ds.vertSize(); r++) {
	    // for each out-direction horizontal row
	  for (int u=0; u<=ds.vertSize(); u++) {
	    w11[band](i_offset+r, j_offset+u) = ds.epsilonDFT(km) * h11(r,u,km,lm)[band];
	    w12[band](i_offset+r, j_offset+u) = ds.epsilonDFT(km) * h12(r,u,km,lm)[band];
	    w21[band](i_offset+r, j_offset+u) = ds.gammaDFT(km) * h21(r,u,km,lm)[band];
	    w22[band](i_offset+r, j_offset+u) = ds.gammaDFT(km) * h22(r,u,km,lm)[band];
	  }
	}

          // into end cap (r==0) is special case cos factor goes to all quads equally (i.e only km=0 is defined)
          // for each out-direction horizontal row
	for (int u=0; u<=ds.vertSize(); u++) {
	  w11[band](i_offset, j_offset+u) = (km==0) ? h11(0,u,km,lm)[band] : 0;
	  w12[band](i_offset, j_offset+u) = 0;
	  w21[band](i_offset, j_offset+u) = 0;
	  w22[band](i_offset, j_offset+u) = 0;
	}

      }
    }
  }
}



// ReflecDFT::copyFromMatrixDFT()
// should check directional structure of rm also

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::copyFromMatrixDFT(const RDD_MatrixDFT& rm) {
    // check band counts are the same
  if (rm.bandCount() != band_count) { jlog::es << "RadDirecDirecDFT::copyMatrixArray() band counts differ"; internalError(); }
    // copy in data from matrix arrays
  copyWholeMatrices(rm.array11(), rm.array12(), rm.array21(), rm.array22()); 
}



// RadDirecDirecDFT::convertToMatrixDFT()

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::copyToMatrixDFT(RDD_MatrixDFT& rm) const { 
    // set up matrix arrays for all four matrices (we assume no decoupling)
  rm.setup4(band_count, ds.lmNumDFT(), ds.rowsDFT());
    // build matrix arrays (size of matrices are set dependent on directonal structure)
  buildWholeMatrices(rm.array11(), rm.array12(), rm.array21(), rm.array22());
}



// RadDirecDirecDFT::diffuseReflectance()

template <typename SD, typename DS> 
const SD RadDirecDirecDFT<SD,DS>::diffuseReflectance() const {

  // see ReflecTable for description of how this is calculated

  SD diffuse_reflec;
  diffuse_reflec.makeZero();
 
  for (int in_index=0; in_index<direcStruct().units(); in_index++) {

      // the radiant power into this quad corresponding to a radiance of 1.0 from all directions (which will integrate to PI)
    //double r_power_in = direcStruct().solidAngleAbsCosTheta(in_index);
    //double r_power_in = 1; // here is radiance to radinace function though
      // here is radiance to radinace scattering function but we are treating like a surface
    double r_power_in = direcStruct().meanCosTheta(in_index);
  
    for (int out_index=0; out_index<direcStruct().units(); out_index++) {
     
        // add power per unit out
      diffuse_reflec.addMultiple(atIndices(in_index, out_index), r_power_in * direcStruct().solidAngleAbsCosTheta(out_index));
      //diffuse_reflec.addMultiple(SD(1), r_power_in * direcStruct().solidAngleAbsCosTheta(out_index));
    }
  }

    // diffuse reflec must now be divided by PI
  diffuse_reflec.divide(M_PI);

  return diffuse_reflec;
}



// RadDirecDirecDFT::display()

template <typename SD, typename DS> 
void RadDirecDirecDFT<SD,DS>::display(JTextStream& str) const {
 
  for (int in_index=0; in_index<ds.units(); in_index++) {
    for (int out_index=0; out_index<ds.units(); out_index++) {
      str.printf("%d %d: ",in_index,out_index);
      atIndices(in_index, out_index).display(str);   
      str << "\n"; 
    }
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECDIRECDFT_H


