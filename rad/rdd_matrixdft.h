
// rdd_matrixdft.h

#ifndef _JUDE_RAD_RDD_MATRIXDFT_H
#define _JUDE_RAD_RDD_MATRIXDFT_H

#include "math/ublas.h"
#include "base/jtextstream.h"
#include "base/jdatastream.h"

using namespace jude::base;
using namespace jude::math;

namespace jude {
namespace rad {

class RD_MatrixDFT;



// This class holds the matrices of the composite forms of the l-mode equations
// as required to specify a hemisphere->hemisphere directional
// reflection or tranmission
// the structure is 4 arrays of square matrices of size rows x l-modes, i.e. a set of four matrices for each band
// E.g. for standard HL hmeisphere discretization of 9 rows (plus cap = 10) by 24 quads, the size is 10 x 13 = 130
// the matrices are laid out as in L&W pg. 418
// w11 holds the cosine amplitudes and for bare slabs this is all that is required, and the matrices are block diagonal
// w11 and w22 are required for boundary reflectances which are rotationally symettrical (and slabs which incorporate such boundaries)
// in this case the matrices are also block diagonal with blocks size rows x rows
// all four matrices are only required for arbitrary functions with no symmetries. If the air=water interface functions are of
// the form described un L&W w12 and w21 are not required, but the marices are not block diagonal but have a checkerboard form.


class RDD_MatrixDFT {

  int band_count;
  int lm_count;
  int row_count;

  matrix<double>* w11;
  matrix<double>* w12;
  matrix<double>* w21;
  matrix<double>* w22;

    // disable assignment operator - deliberately not defined
  const RDD_MatrixDFT& operator=(const RDD_MatrixDFT& rhs);

public:
  
  RDD_MatrixDFT();
  RDD_MatrixDFT(const RDD_MatrixDFT& rm);

  ~RDD_MatrixDFT() { clear(); }

  void copy(const RDD_MatrixDFT& rm);

  int bandCount() const { return band_count; }

  void setup1(int bc, int lc, int rc);
  void setup2(int bc, int lc, int rc);
  void setup4(int bc, int lc, int rc);

  bool isSetup1() const { return (w22==0); }
  bool isSetup2() const { return (w22!=0 && w12==0); }
  bool isSetup4() const { return (w12!=0); }

  void clear();

  matrix<double>* array11() { return w11; }
  matrix<double>* array12() { return w12; }
  matrix<double>* array21() { return w21; }
  matrix<double>* array22() { return w22; }

  const matrix<double>* array11() const { return w11; }
  const matrix<double>* array12() const { return w12; }
  const matrix<double>* array21() const { return w21; }
  const matrix<double>* array22() const { return w22; }

  void translateInToOut(const RD_MatrixDFT& sm, RD_MatrixDFT& res) const;

  void add(const RDD_MatrixDFT& rm);
  void postMultiply(const RDD_MatrixDFT& rm);
  bool makeIdentityMinusInverse() const;

  void writeToDataStream(JDataStream& str) const;
  void readFromDataStream(JDataStream& str);

  bool testIsBlockDiagonal() const;

  void display(JTextStream& str) const;
};



} } // namespace jude::rad

#endif // _JUDE_RAD_RDD_MATRIXDFT_H

