
// rd_matrixdft.h

#include "rad/rd_matrixdft.h"
#include "base/jerror.h"
#include "base/jlog.h"

using namespace jude::rad;



// RD_MatrixDFT::clear()

void RD_MatrixDFT::clear() {
  delete[] m1;
  delete[] m2;
  band_count = 0;
  m1 = 0;
  m2 = 0;
}



// RD_MatrixDFT::setup()

void RD_MatrixDFT::setup(int b) {
  clear();
  band_count = b;
  m1 = new matrix<double>[b];
  m2 = new matrix<double>[b];
}



// RD_MatrixDFT::add()

void RD_MatrixDFT::add(const RD_MatrixDFT& rm) {

  if (band_count != rm.bandCount()) { jlog::es << "RD_MatrixDFT::add() band counts differ.\n"; internalError(); }

  for (int i=0; i<band_count; i++) {
    m1[i] = m1[i] + rm.array1()[i]; 
    m2[i] = m2[i] + rm.array2()[i]; 
  }
}



// RD_MatrixDFT::display()

void RD_MatrixDFT::display(JTextStream& str) const {

  str << "RD_MatrixDFT\n";
  str << "band_count: " << band_count << "\n";

  for (int i=0; i<band_count; i++) {
    str << "band " << i << ":\n";
    str << (m1[i]) << "\n";
    str << (m2[i]) << "\n";
  }
}

