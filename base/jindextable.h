
// jindextable.h

#ifndef _JUDE_BASE_JINDEXTABLE_H
#define _JUDE_BASE_JINDEXTABLE_H

#include "base/jdatastream.h"
#include "base/jtextstream.h"

namespace jude {
namespace base {



/*! \brief Performs conversion from table position (\a x,\a y) to index \a i */

// slow table can have any x-size and y-size
 
class JIndexTable {

  int x_size;    
  int y_size;
    
  int total_size;

public:

  JIndexTable();
  JIndexTable(int xs, int ys) : x_size(xs), y_size(ys), total_size(xs*ys) { }
  JIndexTable(const JIndexTable& st) : x_size(st.x_size), y_size(st.y_size), total_size(st.total_size) { }
  JIndexTable(JDataStream& str) { read(str); }
  JIndexTable(JTextStream& str) { read(str); }

  void setSize(int xs, int ys) {
    x_size = xs;
    y_size = ys;
    total_size = xs*ys;
  }

  int xSize() const { return x_size; }
  int ySize() const { return y_size; }
  int totalSize() const { return total_size; }

  int positionToIndex(int xp, int yp) const { return (yp*x_size) + xp; }

  int wrappedIndexXPlus(int xp, int yp) const { return (yp*x_size) + (xp+1==x_size ? 0 : xp+1); }
  int wrappedIndexXMinus(int xp, int yp) const { return (yp*x_size) + (xp==0 ? x_size-1 : xp-1); }
  int wrappedIndexYPlus(int xp, int yp) const { return ((yp+1==y_size ? 0 : yp+1) * x_size) + xp; }
  int wrappedIndexYMinus(int xp, int yp) const { return ((yp==0 ? y_size-1 : yp-1) * x_size) + xp; }

  void indexToPosition(int i, int& xp, int& yp) const { 
    yp = i / x_size;
    xp = i - (yp*x_size);
  }

  int indexToXPos(int i) const { return i - ((i / x_size) * x_size); }
  int indexToYPos(int i) const { return i / x_size; }

  void read(JDataStream& str);
  void write(JDataStream& str) const;
  void read(JTextStream& str);
  void write(JTextStream& str) const;
};



} } // namespace jude::base

#endif // _JUDE_BASE_JINDEXTABLE_H

