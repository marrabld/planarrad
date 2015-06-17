
// jsortable.h

#ifndef _JUDE_BASE_JSORTABLE_H
#define _JUDE_BASE_JSORTABLE_H

namespace jude {
namespace base {



template <typename T>
class JSortable {

  T v;

public:

  JSortable(const T& v) : v(v) { }
  const T& value() const { return v; }

  static bool compareForSort(const JSortable<T>& a, const JSortable<T>& b) { return (a.v > b.v); }
};



typedef JSortable<double> JSortableDouble;



} } // namespace jude::base

#endif // _JUDE_BASE_JSORTABLE_H

