
// raddirecdirecdftsym.h

#ifndef _JUDE_RAD_RADDIRECDIRECDFTSYM_H
#define _JUDE_RAD_RADDIRECDIRECDFTSYM_H

#include "rad/rdd_matrixdft.h"
#include "math/ublas.h"

using namespace jude::math;

namespace jude {
namespace rad {



/*! \brief A directional-to-directional function converted to DFT representation when the function is 
    dependent only on the angle between incident and exitant vectors. 
*/

template <typename SD, typename DS> 
class RadDirecDirecDFTSym {

  DS ds;
  SD* data;  

  int band_count;
 
  bool lazy_valid;

  int units() const { return ds.rowsDFT() * ds.rowsDFT() * ds.lmNumDFT(); }

public:

  RadDirecDirecDFTSym<SD,DS>(const DS& ds, int bc) : 
    ds(ds), 
    data(new SD[units()]),
    band_count(bc),
    lazy_valid(false) { }

  RadDirecDirecDFTSym<SD,DS>(const RadDirecDirecImp<SD>& ri) :
    ds(ri.direcStruct()), 
    data(new SD[units()]),
    band_count(ri.bandCount()),
    lazy_valid(false) { copyXY(ri); }

  ~RadDirecDirecDFTSym() {
    delete[] data;
  }

  const DS& direcStruct() const { return ds; }

    /*! \brief Returns the number of spectral bands of the data.*/
  int bandCount() const { return band_count; } 

    /*! \brief Copy the data from \a ri converting into DFT representation. */
  bool copyXY(const RadDirecDirecImp<SD>& ri);

  void makeZero() { for (int i=0; i<units(); i++) data[i] = SD(band_count,0); }

  void copy(const RadDirecDirecDFTSym<SD,DS>& rds) {
    assert(ds.isEqual(rds.direcStruct())); 
    SD* to(data);
    const SD* const to_end(data + units());
    const SD* from(rds.data);
    while (to!=to_end) *(to++) = *(from++);
    lazy_valid = rds.lazy_valid;
  }

  void multiply(double rhs) {
    SD* ptr(data);
    const SD* const end(data + units());
    while (ptr!=end) (ptr++)->multiply(rhs);
    lazy_valid = false;
  }

  void multiply(const SD& sd) {
    SD* ptr(data);
    const SD* const end(data + units());
    while (ptr!=end) (ptr++)->multiply(sd);
    lazy_valid = false;
  }

  void divide(double rhs) {
    SD* ptr(data);
    const SD* const end(data + units());
    while (ptr!=end) (ptr++)->divide(rhs);
    lazy_valid = false;
  }

  void divide(const SD& sd) {
    SD* ptr(data);
    const SD* const end(data + units());
    while (ptr!=end) (ptr++)->divide(sd);
    lazy_valid = false;
  }

  void addMultiple(const RadDirecDirecDFTSym<SD,DS>& rds, double m) {
    assert(ds.isEqual(rds.direcStruct())); 
    SD* to(data);
    const SD* const to_end(data + units());
    const SD* from(rds.data);
    while (to!=to_end) (to++)->addMultiple(*(from++),m);
    lazy_valid = false;
  }

  SD& f(int r, int u, int lm) { return data[(r*ds.rowsDFT() + u) * ds.lmNumDFT() + lm]; }
  const SD& f(int r, int u, int lm) const { return data[(r*ds.rowsDFT() + u) * ds.lmNumDFT() + lm]; }

  const SD atIndices(int in_index, int out_index) const;

  void copyToMatrixDFT(RDD_MatrixDFT& rm) const;
 
  void buildIndividualMatrices(matrix<double>* m1, matrix<double>* m2) const;

  void buildWholeMatrices(matrix<double>* w11, matrix<double>* w22) const;

  void display(JTextStream& str) const;
};



// RadDirecDirecDFTSym::copy()

template <typename SD, typename DS> 
bool RadDirecDirecDFTSym<SD,DS>::copyXY(const RadDirecDirecImp<SD>& ri) {
  
  if (!ri.direcStruct().isEqual(ds)) {
    ri.direcStruct().display(jlog::es, 0, ", ");
    ds.display(jlog::es, 0, ", ");
    jlog::es << "RadDirecDirecDFTSym::copy() incompatible directional structure.\n";
    //internalError();
    return false;
  }
  
  if (ds.isFull()) { jlog::ls << "RadDirecDirecDFTSym::copy() not defined for full directional structures.\n"; return false; }
  
  int n = ds.horizSize() / 2;

  // quad to quad scattering

    // for each in-direction horizontal row
  for (int r=1; r<=ds.vertSize(); r++) {
      // for each out-direction horizontal row
    for (int u=1; u<=ds.vertSize(); u++) {
        // for each l-mode
      for (int lm=0; lm<=n; lm++) {

	SD f_sum(band_count, 0);
	for (int v=0; v<ds.horizSize(); v++) {
	  double lm_phi_v = (lm*v*2*M_PI) / ds.horizSize();
	  int in_index = ds.positionToIndexDFT(r,0);
	  int out_index = ds.positionToIndexDFT(u,v);
	  f_sum += ri.atIndices(in_index, out_index) * cos(lm_phi_v);
	}

	f_sum /= ((lm==0 || lm==n) ? ds.horizSize() : n);
	f(r, u, lm) = f_sum;
      }
    }
  }

  // into end cap (r==0) out of a quad (1 <= u <= ds.vertSize())

    // for each out-direction horizontal row
  for (int u=1; u<=ds.vertSize(); u++) {
      // value for l-mode 0 is just any value scattering out at (u) row	
    f(0, u, 0) = ri.atIndices(0, ds.positionToIndexDFT(u,0));
      // for each l-mode 1 to n value is zero
    for (int lm=1; lm<=n; lm++) f(0, u, lm) = SD(band_count, 0);
  }

  // into quad (1 <= u <= ds.vertSize()) out of end cap (u==0)

    // for each in-direction horizontal row
  for (int r=1; r<=ds.vertSize(); r++) {
      // value for l-mode 0 is just any value scattering in from (r) row	
    f(r, 0, 0) = ri.atIndices(ds.positionToIndexDFT(r,0), 0);
      // for each l-mode 1 to n value is zero
    for (int lm=1; lm<=n; lm++) f(r, 0, lm) = SD(band_count, 0);
  }

  // cap to cap
  
    // value for l-mode 0 is just cap to cap value	
  f(0, 0, 0) = ri.atIndices(0, 0);
    // for each l-mode 1 to n value is zero
  for (int lm=1; lm<=n; lm++) f(0, 0, lm) = SD(band_count, 0);

  return true;
}



// RadDirecDirecDFTSym::atIndices()

template <typename SD, typename DS> 
const SD RadDirecDirecDFTSym<SD,DS>::atIndices(int in_index, int out_index) const {

  //int n = ds.horizSize() / 2;

  int r,s,u,v;
 
  ds.indexToPositionDFT(in_index, r, s);
  ds.indexToPositionDFT(out_index, u, v);

    // rotate horizontal position to s = 0 position
  v -= s;
  if (v<0) v += ds.horizSize();

    // angular position
  double phi_v = (v*2*M_PI) / ds.horizSize();

     // sum over each l-mode
  SD sum(band_count, 0);
  for (int lm=0; lm<ds.lmNumDFT(); lm++) sum += f(r,u,lm) * cos(lm * phi_v);

  return sum;
}



// RadDirecDirecDFTSym::buildIndividualMatrices() 
// builds an array of mxm matrices (m = ds.rows) one for each band and each l-mode
// because RadDirecDirecDFTSym holds a function of angle only (in horizontal plane) (ie. is symmetrical in all orientations) 
// this decouples wrt to cos and sin factors and reduces to two arrays instead of the generally required four
// m1 is for the cos factors (= m11) and m2 is for the sin factors (= m22) (m12=0 m21=0)
// these matrices are in the same format as those used to integrate R and T in the PlaneSlab solution (for eg.)
// and so can be used as bottom boundary initialisation for R

template <typename SD, typename DS> 
void RadDirecDirecDFTSym<SD,DS>::buildIndividualMatrices(matrix<double>* m1, matrix<double>* m2) const {

  for (int i=0; i<band_count*ds.lmNumDFT(); i++) {
    m1[i].resize(ds.rowsDFT(), ds.rowsDFT());
    m2[i].resize(ds.rowsDFT(), ds.rowsDFT());
  }

  matrix<double>* curr_m1 = m1;
  matrix<double>* curr_m2 = m2;

  for (int band=0; band<band_count; band++) {

    for (int lm=0; lm<ds.lmNumDFT(); lm++) {

        // for each in-direction horizontal row
      for (int r=1; r<=ds.vertSize(); r++) {
	  // for each out-direction horizontal row
	for (int u=0; u<=ds.vertSize(); u++) {
	  (*curr_m1)(r,u) = ds.epsilonDFT(lm) * f(r,u,lm)[band];
	  (*curr_m2)(r,u) = ds.gammaDFT(lm) * f(r,u,lm)[band];
	}
      }

        // into end cap (r==0) is special case
        // for each out-direction horizontal row
      for (int u=0; u<=ds.vertSize(); u++) {
	(*curr_m1)(0,u) = (lm==0) ? f(0,u,lm)[band] : 0;
	(*curr_m2)(0,u) = 0;
      }

      //printf("band %d lm %d\n",band,lm);
      //std::cout << (*curr_m1) << std::endl;
      //std::cout << (*curr_m2) << std::endl;

      curr_m1++;
      curr_m2++;
    }
  }
}



// fixme
// is untested

template <typename SD, typename DS> 
void RadDirecDirecDFTSym<SD,DS>::buildWholeMatrices(matrix<double>* w11, matrix<double>* w22) const {

  for (int band=0; band<bandCount(); band++) {

    w11[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());
    w22[band].resize(ds.rowsDFT()*ds.lmNumDFT(), ds.rowsDFT()*ds.lmNumDFT());

    for (int lm=0; lm<ds.lmNumDFT(); lm++) {
      int j_offset = lm*ds.rowsDFT();

      int i_offset = j_offset; // ?

        // for each in-direction horizontal row
      for (int r=1; r<=ds.vertSize(); r++) {
	  // for each out-direction horizontal row
	for (int u=0; u<=ds.vertSize(); u++) {
	  w11[band](i_offset+r, j_offset+u) = ds.epsilonDFT(lm) * f(r,u,lm)[band];
	  w22[band](i_offset+r, j_offset+u) = ds.gammaDFT(lm) * f(r,u,lm)[band];
	}
      }

        // into end cap (r==0) is special case
        // for each out-direction horizontal row
      for (int u=0; u<=ds.vertSize(); u++) {
	w11[band](i_offset, j_offset+u) = (lm==0) ? f(0,u,lm)[band] : 0;
	w22[band](i_offset, j_offset+u) = 0;
      }
    }
  }
}



// RadDirecDirecDFTSym::convertToMatrixDFT()

template <typename SD, typename DS> 
void RadDirecDirecDFTSym<SD,DS>::copyToMatrixDFT(RDD_MatrixDFT& rm) const { 
    // set up matrix arrays for two matrices (since in this structure we have rotational symmetry)
  rm.setup2(bandCount(), ds.lmNumDFT(), ds.rowsDFT());
    // build matrix arrays (size of matrices are set dependent on directonal structure)
  buildWholeMatrices(rm.array11(), rm.array22());
}



// RadDirecDirecDFTSym::display()

template <typename SD, typename DS> 
void RadDirecDirecDFTSym<SD,DS>::display(JTextStream& str) const {

  str << "band_count: " << band_count << "\n";

  for (int in_index=0; in_index<ds.units(); in_index++) {
    for (int out_index=0; out_index<ds.units(); out_index++) {
      str << in_index << " " << out_index << " : [ " << atIndices(in_index, out_index) << " ]\n";
    }
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_RADDIRECDIRECDFTSYM_H

