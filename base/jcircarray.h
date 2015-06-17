
// jcircarray.h

#ifndef _JUDE_BASE_CIRCARRAY_H
#define _JUDE_BASE_CIRCARRAY_H

namespace jude {
namespace base {



/*! A fixed size array for type T. The write and read positions for objects move around the areas in a circular first-in first-out manner.

  The array is constructed to contain a set number of objects of type T, each constructed at creation time with their
  default constructor, T(). At any time only a subset in the array are considered valid, initially none. The class maintains 
  two pointers, the current read position and teh current write position. To add an object to the array write to 
  the object using currWriteObject() and the increment the current write position using incWritePos(). The read an object, access 
  the object at currReadObject() then increment the read position incReadPos(). The read an write positions move circularly around 
  the array, going back to the beginning when they reach the end. 

  Care is required when using this array type since underflows and overflows can occur without any obvious error since reading 
  writing is always allowed. Always ensure reading an object is valid by checking the array is not empty with isEmpty() beforehand
  If the array capacity is exceeded, i.e. the write position catches up with read position, then an overflow flag 
  is set which can be checked with hadOverflow(). While the array is full calls to incWritePos() are ignored so the
  last object to be inserted will be overwritten.

  Note that this class is not thread-safe.

  \todo There are many ways to get in trouble when using this class, its operation and interface should be reviewed.
*/

template <typename T>
class JCircArray {

  T* array_obj;
  int array_size;

  int curr_read_pos;
  int curr_write_pos;

  bool had_overflow;

public:

    /*! \brief Construct an array of a fixed number of type \a T objects, created with their default constructor. */ 
  JCircArray(int n) : 
    array_obj(new T[n]), 
    array_size(n), 
    curr_read_pos(0),
    curr_write_pos(0),
    had_overflow(false) { }

  ~JCircArray() { delete[] array_obj; }

    /*! \brief Returns \c true if the array has no valid objects in it. */
  bool isEmpty() const { return curr_read_pos == curr_write_pos; }

    /*! \brief Returns \c true if the array has no valid objects in it. */
  int objectCount() const { 
    return (curr_read_pos <= curr_write_pos) ? curr_write_pos - curr_read_pos : array_size - curr_read_pos - curr_write_pos; 
  }

    /*! \brief Returns \c true if this array has ever had too many objects written at once. */
  bool hadOverflow() const { return had_overflow; }
    /*! \brief Clears the flag that records if this array has ever had too many objects written at once. */
  void clearHadOverflow() { had_overflow = false; }

    /*! \brief Accesses the object at the current read position. */
  const T& currReadObject() const { return array_obj[curr_read_pos]; };

    /*! \brief Accesses the object at the current write position. */
  T& currWriteObject() { return array_obj[curr_write_pos]; };

    /*! \brief Moves the read position one forward in the array. */
  void incReadPos() { 
    if (isEmpty()) { had_overflow = true; return; }
    curr_read_pos++;
    if (curr_read_pos == array_size) curr_read_pos = 0;
  }

    /*! \brief Moves the write position one forward in the array. */
  void incWritePos() { 
    int p = curr_write_pos + 1;
    if (p == array_size) p = 0;
    if (p == curr_read_pos) had_overflow = true;
    else curr_write_pos = p;
  }
};



} } // namespace jude::base

#endif // _JUDE_BASE_CIRCARRAY_H

