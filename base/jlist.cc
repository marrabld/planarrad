
// jlist.cc

#include "jlist.h"
#include <stdio.h>

using namespace jude::base;



// the actual static memory pool of jlist elements

//JMemPool JList_Base::elem::se_pool = JMemPool(sizeof(JList_Base::elem), 64);



// JList_Base::constructor()
// construct and initialise a linked list of pointers

JList_Base::JList_Base() {
  first_elem = 0;
  last_elem = 0;
  curr_elem = 0;
  n = 0;
  auto_delete = false;
}



// JList_Base::autoDelete()

bool JList_Base::autoDelete() const {
  return auto_delete;
}



// JList_Base::setAutoDelete(bool f)

void JList_Base::setAutoDelete(bool enable) {
  auto_delete = enable;
}



// JList_Base::count()

int JList_Base::count() const { 
  return n; 
}



// JList_Base::copyReferences()
// already cleared by JList

void JList_Base::copyReferences(const JList_Base& rhs) {
    // get first elem to copy or quit
  elem* from_elem = rhs.first_elem;
  if (!from_elem) return;
    // copy first element
  first_elem = new elem(from_elem->ref, 0);
  curr_elem = first_elem;
    // get next source element
  from_elem = from_elem->next;
    // until source exhausted
  while (from_elem) {
      // create new element
    elem* e = new elem(from_elem->ref, curr_elem);
    curr_elem->next = e;
    curr_elem = e;
      // get next source element
    from_elem = from_elem->next;
  }  
  curr_elem->next = 0;
  last_elem = curr_elem;
  n = rhs.n;
}



// JList_Base::at()

void* JList_Base::at(int i) const {
    // quit if out of range
  if (i<0 || i>n-1) return 0;
    // if i is before halfway then start at the first and go forward
  if (i <= (n>>1)) {
    curr_elem = first_elem;
    for (;i>0;i--) curr_elem = curr_elem->next;
    // otherwise i is over halfway so start at the end and work back
  } else {
    curr_elem = last_elem; 
    for (;i<n-1;i++) curr_elem = curr_elem->prev;
  }
  return curr_elem->ref;
}



// JList_Base::first()

void* JList_Base::first() const {
  curr_elem = first_elem;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JList_Base::second()

void* JList_Base::second() const {
  curr_elem = first_elem;
  if (!first_elem) return 0;
  curr_elem = first_elem->next;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JList_Base::last()

void* JList_Base::last() const {
  curr_elem = last_elem;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JList_Base::next()

void* JList_Base::next() const {
  if (!curr_elem) return 0;
  curr_elem = curr_elem->next;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JList_Base::prev()

void* JList_Base::prev() const {
  if (!curr_elem) return 0;
  curr_elem = curr_elem->prev;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JList_Base::current()

void* JList_Base::current() const {
  if (!curr_elem) return 0;
  return curr_elem->ref;
}



// JList_Base::replace()

void JList_Base::replace(void* item) {
  if (curr_elem) curr_elem->ref = item;
}

    

// JList_Base::append()

void JList_Base::append(void* item) {
  elem* e = last_elem;
  elem* ne = new elem();
  ne->ref = item;
  ne->prev = e;
  ne->next = 0;
  if (e) e->next = ne;
  else first_elem = ne;
  last_elem = ne;
  n++;
}



// JList_Base::prepend()

void JList_Base::prepend(void* item) {
  elem* e = first_elem;
  elem* ne = new elem();
  ne->ref = item;
  ne->prev = 0;
  ne->next = e;
  if (e) e->prev = ne;
  else last_elem = ne;
  first_elem = ne;
  n++;
}


// JList_Base::insert()

void JList_Base::insert(void *item, int i) {
    // if at start of list
  if (i<=0) return prepend(item);
    // if at end of list
  if (i>n-1) return append(item);
    // if i is before halfway then start at the first and go forward
  if (i <= (n>>1)) {
    curr_elem = first_elem;
    for (;i>0;i--) curr_elem = curr_elem->next;
    // otherwise i is over halfway so start at the end and work back
  } else {
    curr_elem = last_elem; 
    for (;i<n-1;i++) curr_elem = curr_elem->prev;
  }
    // got item at position (i) so insert new item before it
  elem* ne = new elem();
  ne->ref = item;
  ne->prev = curr_elem->prev;
  ne->next = curr_elem;
  ne->prev->next = ne;
  curr_elem->prev = ne;
  n++;
}



// JList_Base::insertAtCurrentPos()
// note can never put at end - use append()

void JList_Base::insertAtCurrentPos(void *item) {
    // if at start of list
  if (curr_elem == first_elem) return prepend(item);
    // insert new item before current element
  elem* ne = new elem();
  ne->ref = item;
  ne->prev = curr_elem->prev;
  ne->next = curr_elem;
  ne->prev->next = ne;
  curr_elem->prev = ne;
  n++;
}



// JList_Base::remove()
// removes the first occurrence of item
// returns true if the item was found, false if it was not found in the list
// if auto-delete is enabled then item is also deleted if it is found

bool JList_Base::remove(void* item) {
  elem* e = first_elem;
  while (e) { 
    if (e->ref==item) break; 
    e = e->next;
  }
  if (!e) return false;

  if (e->prev) e->prev->next = e->next;
  else first_elem = e->next;
  if (e->next) e->next->prev = e->prev;
  else last_elem = e->prev;
  delete e;
  n--;
  return true;
}




// JList_Base::removeAtCurrentPos()
// moves the current position on to the next item
// unless was already at the end
// note can never put at end - use append()

JList_Base::elem* JList_Base::removeAtCurrentPos() {

  if (curr_elem==0) return 0;

  elem* e_del = curr_elem;
  n--;
  
    // if took the last element
  if (n==0) {
    first_elem = 0;
    last_elem = 0;
    curr_elem = 0;

    // if at start of list
  } else if (curr_elem == first_elem) {
    first_elem = curr_elem->next;
    curr_elem->next->prev = 0;
    curr_elem = first_elem;;

    // if at end of list
  } else if (curr_elem == last_elem) {
    last_elem = curr_elem->prev;
    curr_elem->prev->next = 0;
    curr_elem = 0;

  } else {
    curr_elem->prev->next = curr_elem->next;
    curr_elem->next->prev = curr_elem->prev;
    curr_elem = curr_elem->next;
  }

  return e_del;
}





// JList_Base::find()
// finds the first occurrence of item
// returns the position of the item if it was found, or -1 if it was not found in the list

int JList_Base::find(const void* item) const {
  elem* e = first_elem;
  int i = 0;
  while (e) { 
    if (e->ref==item) break; 
    i++;
    e = e->next;
  }
  if (!e) return -1;
  curr_elem = e;
  return i;
}



// JList_Base::toFirst()

bool JList_Base::toFirst(void* item) {

  if (find(item) < 0) return false;

  if (curr_elem == first_elem) return true;

  elem* pe = curr_elem->prev;
  elem* ne = curr_elem->next;

  curr_elem->next = first_elem;
  curr_elem->prev = 0;
  first_elem->prev = curr_elem;  
  first_elem = curr_elem;

    // note pe != 0 always 
  pe->next = ne;
  if (ne) ne->prev = pe;
  else last_elem = pe;

  return true;
}
  


// JList_Base::toLast()

bool JList_Base::toLast(void* item) {

  if (find(item) < 0) return false;

  if (curr_elem == last_elem) return true;

  elem* pe = curr_elem->prev;
  elem* ne = curr_elem->next;
  
  curr_elem->next = 0;
  curr_elem->prev = last_elem;
  last_elem->next = curr_elem;  
  last_elem = curr_elem;

    // note ne must be != 0
  if (pe) pe->next = ne;
  else first_elem = ne;
  ne->prev = pe;

  return true;
}



// JList_Base::appendUnique()

void JList_Base::appendUnique(void* item) {
  if (find(item) < 0) append(item);
}



// JList_Base::buildArrayFromList()

void JList_Base::buildArrayFromList(void** array) {
  elem* e = first_elem;
    // copy subsequent elements to end or max_len
  while (e) { 
    *(array++) = e->ref; 
    e = e->next;
  }
}



// JList_Base::buildArrayFromList()

void JList_Base::buildArrayFromList(void** array, int start, int max_len) {
  elem* e = first_elem;
    // jump past first elements to ignore
  while (e && start > 0) { e = e->next; start--; }
    // copy subsequent elements to end or max_len
  while (e && max_len > 0) { 
    *(array++) = e->ref; 
    e = e->next;
    max_len--;
  }
}



// JList_Base::buildListFromArray()

void JList_Base::buildListFromArray(void** array, int len) {

  elem* e = first_elem;

    // delete the pointer blocks
  while (e) { 
    delete e->prev;
    e = e->next; 
  }
  delete e;

    // create first element
  first_elem = new elem(*(array++), 0);
  curr_elem = first_elem;
    // create other elements
  for (int i=1;i<len;i++) {
    curr_elem->next = new elem(*(array++), curr_elem);
    curr_elem = curr_elem->next;
  }

  curr_elem->next = 0;
  last_elem = curr_elem;
  curr_elem = first_elem;
  n = len;
}



// JListIterator_Base::current()

void* JListIterator_Base::current() {
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JListIterator_Base::first()

void* JListIterator_Base::first() {
  curr_elem = lb.first_elem;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JListIterator_Base::last()

void* JListIterator_Base::last() {
  curr_elem = lb.last_elem;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JListIterator_Base::next()

void* JListIterator_Base::next() {
  if (!curr_elem) return 0;
  curr_elem = curr_elem->next;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}



// JListIterator_Base::prev()

void* JListIterator_Base::prev() {
  if (!curr_elem) return 0;
  curr_elem = curr_elem->prev;
  if (curr_elem) return curr_elem->ref;
  else return 0; 
}





