
// dsquadipol.h

#ifndef _JUDE_DS_DSQUADIPOL_H
#define _JUDE_DS_DSQUADIPOL_H

namespace jude {
namespace ds {



class DsQuadIpol {

public:

  int n00;
  int n01;
  int n02;
  int n10;
  int n11;
  int n12;
  int n20;
  int n21;
  int n22;

  double frac00;
  double frac01;
  double frac02;
  double frac10;
  double frac11;
  double frac12;
  double frac20;
  double frac21;
  double frac22;

  template <typename A>
  A buildData(const A* data) {
    return data[n00]*frac00 + data[n01]*frac01 + data[n02]*frac02 + \
           data[n10]*frac10 + data[n11]*frac11 + data[n12]*frac12 + \
           data[n20]*frac20 + data[n21]*frac21 + data[n22]*frac22;
  }

  double fracSum() const { return frac00 + frac01 + frac02 + frac10 + frac11 + frac12 + frac20 + frac21 + frac22; }

  /*
  int index0;
  int index1;
  int index2;
  int index3;
 
  double frac0;
  double frac1;
  double frac2;
  double frac3;
  
  template <typename A>
  A buildData(const A* data) {
    return data[index0]*frac0 + data[index1]*frac1 + data[index2]*frac2 + data[index3]*frac3;
  }

  double fracSum() const { return frac0 + frac1 + frac2 + frac3; }

  */
};



} } // namespace jude::ds

#endif // _JUDE_DS_DSQUADIPOL_H

