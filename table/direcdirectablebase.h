
// direcdirectablebase.h

#ifndef _JUDE_TABLE_DIRECDIRECTABLEBASE_H
#define _JUDE_TABLE_DIRECDIRECTABLEBASE_H

namespace jude {
namespace table {



template <typename A>
class DirecDirecTableBase {

public:

  typedef A data_type;

  DirecDirecTableBase<A>() { }
  virtual ~DirecDirecTableBase() { }

  virtual A* dataArray() = 0;
  virtual const A* dataArray() const = 0;

  //duplicateStructure()
};



} } // namespace jude::table

#endif // _JUDE_TABLE_DIRECDIRECTABLEBASE_H

