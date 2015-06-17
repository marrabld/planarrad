
// jindextable.cc

#include "jindextable.h"

using namespace jude::base;



void JIndexTable::read(JDataStream& str) {
  str >> x_size;
  str >> y_size;
  total_size = x_size * y_size;
}



void JIndexTable::write(JDataStream& str) const {
  str << x_size;
  str << y_size;
}



/*!
\todo Not yet implemented. Requires JTextStream operator()>> to double.
*/

void JIndexTable::read(JTextStream& str) {
  //str >> x_size;
  //str >> y_size;
  //total_size = x_size * y_size;
}



void JIndexTable::write(JTextStream& str) const {
  str << x_size;
  str << y_size;
}

