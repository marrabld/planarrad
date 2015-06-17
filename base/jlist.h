
#ifndef _JUDE_BASE_LIST_H
#define _JUDE_BASE_LIST_H

//#include "jmempool.h"
#include <stdio.h>

namespace jude {
namespace base {



// disable "no matching operator delete" warning on vc++ 6.0
#ifdef VC6
#pragma warning(disable:4291)
#endif



// JList_Base
// base class for all JList specialisations operates on (void*)

// prevent Doxygen from documenting internal implementation
/// @cond

class JList_Base {

protected:

    // fixme 
    // here is a problem because it seems the JMemPool can be destroyed before
    // the JLst destructor is called - when auto delete tries to traverse the list it accesses free'd memory
  class elem {
    //static JMemPool se_pool;
  public:
    void* ref;
    elem* prev;
    elem* next;
    elem() { }
    elem(void* ref, elem* prev) : ref(ref), prev(prev) { }
    //static void* operator new(size_t n) { return se_pool.alloc(n); }
    //static void operator delete(void *p, size_t n) { se_pool.free(p,n); }
  };

  elem* first_elem;
  elem* last_elem;
  mutable elem* curr_elem;
  int n;

  bool auto_delete;

public:

  JList_Base();
 
  bool autoDelete() const;
  void setAutoDelete(bool enable);

  int count() const;

  void copyReferences(const JList_Base& rhs);

  void* at(int i) const;
  void* first() const;
  void* second() const;
  void* last() const;
  void* next() const;
  void* prev() const;
  void* current() const;

  void append(void* item);
  void prepend(void* item);
  void replace(void* item);
  void insert(void* item, int i);
  void insertAtCurrentPos(void *item);
  bool remove(void* item);
  elem* removeAtCurrentPos();
  int find(const void* item) const;

  bool toFirst(void* item);
  bool toLast(void* item);
 
  void appendUnique(void* item);
 
  void buildListFromArray(void** array, int len);
  void buildArrayFromList(void** array);
  void buildArrayFromList(void** array, int start, int max_len);

  //void buildListFromArray(const JArray& ja);

  friend class JListIterator_Base;
};



// JListIterator_Base

class JListIterator_Base {

  const JList_Base& lb;
  JList_Base::elem* curr_elem;

public:

  JListIterator_Base(const JList_Base& lb) : lb(lb), curr_elem(lb.first_elem) { }

  void* current();
  void* first();
  void* last();
  void* next();
  void* prev();
};


/// @endcond



/*!
\brief Manages a linked list of pointers to the referenced class type.
*/

template <class A> 
class JList : public JList_Base {

  inline void arraySwap(A*& a, A*& b) { A* t=a; a=b; b=t; }

  void fqsortCall(A** a, int l, int r);
  void insortCall(A** a, int lo, int hi);

public:

  JList() { }
  JList(A* a) { append(a); }
  JList(A* a0, A* a1) { append(a0); append(a1); }
  JList(A* a0, A* a1, A* a2) { append(a0); append(a1); append(a2); }
  JList(A* a0, A* a1, A* a2, A* a3) { append(a0); append(a1); append(a2); append(a3); }
  JList(A* a0, A* a1, A* a2, A* a3, A* a4) { append(a0); append(a1); append(a2); append(a3); append(a4); }
  JList(A* a0, A* a1, A* a2, A* a3, A* a4, A* a5) { append(a0); append(a1); append(a2); append(a3); append(a4); append(a5); }

  ~JList() { clear(); }

  bool autoDelete() const { return JList_Base::autoDelete(); }
  void setAutoDelete(bool enable) { JList_Base::setAutoDelete(enable); }

  int count() const { return JList_Base::count(); }

  void clear();

  void copyReferences(const JList<A>& rhs) {
    clear(); 
    JList_Base::copyReferences(rhs); 
  }

  A* at(int i) const { return (A*) JList_Base::at(i); }
  A* first() const { return (A*) JList_Base::first(); }
  A* second() const { return (A*) JList_Base::second(); }
  A* last() const { return (A*) JList_Base::last(); }
  A* next() const { return (A*) JList_Base::next(); }
  A* prev() const { return (A*) JList_Base::prev(); }
  A* current() const { return (A*) JList_Base::current(); }

  void append(A* item) { JList_Base::append((void*)item); }
  void prepend(A* item) { JList_Base::prepend((void*)item); }
  void replace(A* item) { JList_Base::replace((void*)item); }
  void insert(A* item, int i) { JList_Base::insert((void*)item, i); }
  void insertAtCurrentPos(A *item) { JList_Base::insertAtCurrentPos((void*)item); }
  int sortedPosition(A* item) const;

  bool remove(A* item) { 
    bool r = JList_Base::remove((void*)item);
    if (auto_delete && r) delete item;
    return r;
  }

  A* removeAtCurrentPos() { 
    elem* e = JList_Base::removeAtCurrentPos();
    if (auto_delete && e) delete ((A*) e->ref);
    delete e;
    return current();
  }

  int find(const A* item) const { return JList_Base::find((void*)item); }

  void sort();

  bool toFirst(A* item) { return (A*) JList_Base::toFirst((void*)item); }
  bool toLast(A* item) { return (A*) JList_Base::toLast((void*)item); }
 
  void appendUnique(A* item) { JList_Base::appendUnique((void*)item); }
};



// JList::clear()
// clear() is part of the template class because need to know what the items are
// to delete them (maybe not but technically delete *void is undefined)

template <class A> void JList<A>::clear() {

  elem* e = first_elem;
  elem* pe;

    // if not auto deleting entries then just delete the pointer blocks
  if (!auto_delete) {
    while (e) { 
      pe = e; 
      e = e->next; 
      delete pe;
    }
 
    // otherwise delete what is pointed to also
  } else {
    while (e) { 
      pe = e; 
      e = e->next;
      delete (A*) pe->ref; 
      delete pe; 
    }
  }

  first_elem = 0;
  last_elem = 0;
  curr_elem = 0;
  n = 0;
}



// JList::fqsortCall() [private]
// A::compareForSort(const A& a, const A& b) returns true if a>b to sort into acsending order

template <class A>
void JList<A>::fqsortCall(A** a, int l, int r) {
    
  if ((r-l) <= 4) return;
    
  int i = (r+l) >> 1;
    
  if (A::compareForSort(*a[l], *a[i])) { arraySwap(a[l], a[i]); }
  if (A::compareForSort(*a[l], *a[r])) { arraySwap(a[l], a[r]); }
  if (A::compareForSort(*a[i], *a[r])) { arraySwap(a[i], a[r]); }
    
  int j = r-1;
  arraySwap(a[i], a[j]);
  i = l;
  int pj = j;

  for (;;) {
    while (A::compareForSort(*a[pj], *a[++i]));
    while (A::compareForSort(*a[--j], *a[pj]));
    if (j < i) break;
    arraySwap(a[i], a[j]);
  }		

  arraySwap(a[i], a[r-1]);

  fqsortCall(a, l, j);
  fqsortCall(a, i+1, r);
}



// JList::insortCall() [private]

template <class A>
void JList<A>::insortCall(A** a, int lo, int hi) {

  for (int i=lo+1; i<=hi; i++) {
    A* temp = a[i];
    int j = i;
    while (j>lo && A::compareForSort(*a[j-1], *temp)) { a[j] = a[j-1]; j--; }
    a[j] = temp;
  }
}



// JList::sort()

template <class A>
void JList<A>::sort() {

  int len = count();

  if (len < 2) return;

    // build array of pointers
  A** array = new A*[len];
  buildArrayFromList((void**)array);

    // do quicksort on the array
  fqsortCall(array,0,len-1);
  insortCall(array,0,len-1);

    // rebuild the list from the sorted pointer array
  buildListFromArray((void**)array, len);

    // delete the pointer array
  delete[] array;
}



// JList<A>::sortedPosition()

template <class A>
int JList<A>::sortedPosition(A* item) const {

  if (count() == 0) return 0;
 
    // the first jump size
  int pos = 1;
  while ((pos<<1) <= count()) pos = pos << 1;
  int jump = pos >> 1;
  pos--;

  curr_elem = first_elem;
  int curr_pos = 0;

  while (jump > 0) {

    //printf("pos %d jump %d count %d\n", pos, jump, count());

      // if out of bounds move back
    if (pos >= count()) pos = pos - jump;
    else {
        // move to required position
      for (; curr_pos<pos; curr_pos++) curr_elem = curr_elem->next;
      for (; curr_pos>pos; curr_pos--) curr_elem = curr_elem->prev;
        // test if need to move forward or back
      //printf("comp %d\n", A::compareForSort(*item, *((A*) (curr_elem->ref))));
      pos = (A::compareForSort(*item, *((A*) (curr_elem->ref)))) ? curr_pos + jump : curr_pos - jump;
    }
    jump = jump >> 1;
  }
  
    // move to required position
  for (; curr_pos<pos; curr_pos++) curr_elem = curr_elem->next;
  for (; curr_pos>pos; curr_pos--) curr_elem = curr_elem->prev;
 
  //if (pos < count()) printf("final comp %d %d\n", pos, A::compareForSort(*item, *((A*) (curr_elem->ref))));
   
    // might need to go before or after final found item 
  if (pos < count()) if (A::compareForSort(*item, *((A*) (curr_elem->ref)))) { pos++; curr_elem = curr_elem->next; }

  return pos;
}



/*!
\brief Iterator on a JList.

This is the preferred way to traverse a list because JList::first() and JList::next() are not thread-safe.
*/

template <class A> class JListIterator : protected JListIterator_Base {

public:

  JListIterator(const JList<A>& lb) : JListIterator_Base(lb) { }

  A* current() { return (A*) JListIterator_Base::current(); }
  A* first() { return (A*) JListIterator_Base::first(); }
  A* last() { return (A*) JListIterator_Base::last(); }
  A* next() { return (A*) JListIterator_Base::next(); }
  A* prev() { return (A*) JListIterator_Base::prev(); }
};



} } // namespace  jude::base

#endif // _JUDE_BASE_LIST_H

