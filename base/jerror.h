
// jerror.h

#ifndef _JUDE_BASE_JERROR_H
#define _JUDE_BASE_JERROR_H

#include <stdlib.h>

#ifdef NDEBUG
#define jassert(f) f
#else
#define jassert(f) true
#endif

namespace jude {
namespace base {


/*! \brief An internal error is an error which should never occur regardless of user input.

Call this function if an error is detected which cannot be recovered from. The program will exit. 
Programs should pre-check all values to ensure the internal error condition is never met. 
Situations for which this is not feasible should not be considered internal errors, but rather 
operational errors, and so should be designed to recover and not call this function.

\todo Currently this just calls exit(). Probably needs to do something more clever for gui environment.
Best idea is for the program to set a static flag if is in gui mode in order to produce a window.
*/

inline void internalError() { abort(); }



} } // namespace jude::base

#endif // _JUDE_BASE_JERROR_H

