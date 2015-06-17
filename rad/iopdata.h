
// iopdata.h

#ifndef _JUDE_RAD_IOPDATA_H
#define _JUDE_RAD_IOPDATA_H

#include "math/ublas.h"

using namespace jude::math;

namespace jude {
namespace rad {



template <typename SD, typename HDS>
class IOPData {

protected:

  const HDS hds;

  double kdelta(int i) { return (i==0) ? 1 : 0; }

  IOPData(const HDS& hds) : 
    hds(hds) { }

  template <typename SD2>
  void calcTauAndRho(matrix<double>& tau, matrix<double>& rho, 
		     const RadDirecDirecDFTSym<SD2, HDS>& Bplus_dft, 
		     const RadDirecDirecDFTSym<SD2, HDS>& Bminus_dft, int band, int lm);

public:

  // depreceated
  const HDS& halfDirecStruct() { return hds; }

  const HDS& direcStruct() { return hds; }

};




// IOPData::buildTauAndRho() [protected]
// L&W pg. 400

template <typename SD, typename HDS> 
template <typename SD2> 
void IOPData<SD,HDS>::calcTauAndRho(matrix<double>& tau, matrix<double>& rho, 
				    const RadDirecDirecDFTSym<SD2, HDS>& Bplus_dft, 
				    const RadDirecDirecDFTSym<SD2, HDS>& Bminus_dft, int band, int lm) {

  // quad to quad

    // loop through in-direction rows
  for (int r=1; r<=hds.vertSize(); r++) {
      //loop through out-direction rows
    for (int u=1; u<=hds.vertSize(); u++) {

        // mu_u ie. cos theta for this out direction
      double ct = hds.meanCosTheta(hds.positionToIndexDFT(u,0));

      tau(r,u) = (hds.epsilonDFT(lm) * Bplus_dft.f(r,u,lm)[band] - kdelta(r-u)) / ct;
      rho(r,u) = (hds.epsilonDFT(lm) * Bminus_dft.f(r,u,lm)[band]) / ct;
    }
  }

  // cap to quad (r=0)

    // loop through out-direction rows
  for (int u=1; u<=hds.vertSize(); u++) {

      // mu_u ie. cos theta for this out direction
    double ct = hds.meanCosTheta(hds.positionToIndexDFT(u,0));

    if (lm==0) {
      tau(0,u) = Bplus_dft.f(0,u,0)[band] / ct;
      rho(0,u) = Bminus_dft.f(0,u,0)[band] / ct;
    } else {
      tau(0,u) = 0;
      rho(0,u) = 0;
    }
  }

  // quad to cap (u=0)

    // mu_u ie. cos theta for this out direction
  double ct = hds.meanCosTheta(hds.positionToIndexDFT(0,0));

    // loop through out-direction rows
  for (int r=1; r<=hds.vertSize(); r++) {

    if (lm==0) {
      tau(r,0) = hds.epsilonDFT(0) * Bplus_dft.f(r,0,0)[band] / ct;
      rho(r,0) = hds.epsilonDFT(0) * Bminus_dft.f(r,0,0)[band] / ct;
    } else {
      tau(r,0) = 0;
      rho(r,0) = 0;
    }
  }

  // cap to cap (r=0,u=0)
  
  if (lm==0) {
    tau(0,0) = (Bplus_dft.f(0,0,0)[band] - 1) / ct;
    rho(0,0) = Bminus_dft.f(0,0,0)[band] / ct;
  } else {
    tau(0,0) = 0;
    rho(0,0) = 0;
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_IOPDATA_H


