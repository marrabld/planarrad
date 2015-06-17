
// rd_matrixdft.h

#ifndef _JUDE_RAD_RD_MATRIXDFT_H
#define _JUDE_RAD_RD_MATRIXDFT_H

#include "math/ublas.h"
#include "base/jtextstream.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace rad {



class RD_MatrixDFT {

  int band_count;

    // in fact these are just vectors - any advantage to use vector type ?
  matrix<double>* m1;
  matrix<double>* m2;

    // disable assignment operator - deliberately not defined
  const RD_MatrixDFT& operator=(const RD_MatrixDFT& rhs);

public:
  
  RD_MatrixDFT() :
    band_count(0),
    m1(0),
    m2(0) { }

  ~RD_MatrixDFT() {
    clear();
  }

  int bandCount() const { return band_count; }

  void setup(int b);
  void clear();

  matrix<double>* array1() { return m1; }
  matrix<double>* array2() { return m2; }

  const matrix<double>* array1() const { return m1; }
  const matrix<double>* array2() const { return m2; }

  void add(const RD_MatrixDFT& sm);

  void display(JTextStream& str) const;
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RD_MATRIXDFT_H

