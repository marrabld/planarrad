
#ifndef _JUDE_BASE_JSTACK_H
#define _JUDE_BASE_JSTACK_H

namespace jude {
namespace base {

#include <stdio.h>



// disable "no matching operator delete" warning on vc++ 6.0
//#ifdef VC6
//#pragma warning(disable:4291)
//#endif



template <typename A> class JStack {

  class _Stack_Elem {
  public:
    A v;
    _Stack_Elem* prev;
    _Stack_Elem(A v, _Stack_Elem* prev) : v(v), prev(prev) { }
  };

  _Stack_Elem *curr;

  int curr_count;

public:

  JStack<A>() : curr(0), curr_count(0) { }
  ~JStack() { clear(); }

  int count() const { return curr_count; }

  void clear() { 
    while (curr) { 
      _Stack_Elem* e = curr->prev;
      delete curr;
      curr = e;
    }
    curr_count = 0;
  }

  bool isEmpty() const { return (curr==0); }

  void push(const A& v) { 
    curr = new _Stack_Elem(v,curr); 
    curr_count++; 
  }

  A pop() { 
    _Stack_Elem* e = curr;
    curr = e->prev;
    A v = e->v;
    delete e;
    curr_count--;
    return v;
  }
};



} } // namespace jude::base

#endif // _JUDE_BASE_JSTACK_H

