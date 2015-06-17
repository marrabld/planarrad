
// quaddatasource.h

#ifndef _JUDE_DS_QUADDATASOURCE_H
#define _JUDE_DS_QUADDATASOURCE_H

#include "base/jtextstream.h"

namespace jude {
namespace ds {



/*! \brief Interface for classes that retrieve info to display in text output of a directional data table. */
 
class QuadDataSource {

public:

  virtual ~QuadDataSource() { }

  virtual void displayAtIndex(JTextStream& str, int index, const JString& sub_sep) const = 0;
};



class QuadDataWrite {

public:

  virtual ~QuadDataWrite() { }
 
  virtual void setAtIndex(int index, double v) const = 0;
};



} } // namespace jude::ds

#endif // _JUDE_DS_QUADDATASOURCE_H

