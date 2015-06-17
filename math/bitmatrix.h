
// bitmatrix.h

#ifndef _JUDE_MATH_BITMATRIX_H
#define _JUDE_MATH_BITMATRIX_H

#include "base/jtypes.h"
#include "math/rand.h"

namespace jude {
namespace math {


class BitMatrix {

  int s1;
  int s2;

  int word_count;

  uns32* word_data;

  void realloc(int wc) {
    delete[] word_data;
    word_data = new uns32[wc];
    word_count = wc;
  }

public:

  BitMatrix(int s1, int s2) :
    s1(s1),
    s2(s2),
    word_count((s1 * s2 + 31) >> 5),
    word_data(new uns32[word_count]) { }

  ~BitMatrix() { delete[] word_data; }


  bool bit(int i1, int i2) const { 
    int i = i1*s2 + i2;
    return ((word_data[i>>5] & (1 << (i&31))) != 0);
  }

  void setBit(int i1, int i2, bool b) { 
    int i = i1*s2 + i2;
    int offset = i&31;
    i = i >> 5;
    word_data[i] = (word_data[i] & (~(1 << offset))) | ((b ? 1 : 0) << offset);
  }

  void makeAllZero() {
    for (int i=0; i<word_count; i++) word_data[i] = 0;
  }

  void makeAllOne() {
    for (int i=0; i<word_count; i++) word_data[i] = 0xffffffff;
  }

  void randomise() {
    for (int i=0; i<word_count; i++) {
      uns32 v = 0;
      for (int j=0; j<32; j++) if (rand_bool_cstd()) v |= (1<<j); 
      word_data[i] = v;
    }
  }

  void write(JDataStream& str) const {
    str << s1;
    str << s2;
    str << word_count;
    for (int i=0; i<word_count; i++) str << word_data[i];
  } 

  void read(JDataStream& str) {
    str >> s1;
    str >> s2;
    int wc;
    str >> wc;
    if (wc != word_count) realloc(wc);
    for (int i=0; i<word_count; i++) str >> word_data[i];
  } 

};



} } // namespace jude::math

#endif // _JUDE_MATH_BITMATRIX_H

