
// jchar.h

#ifndef _JUDE_BASE_CHAR_H
#define _JUDE_BASE_CHAR_H

#include "base/jtypes.h"

namespace jude {
namespace base {



// the qt binding assumes that a JChar is exactly the same as a QChar
// such that they can be cast interchangably

/*!
\brief A 16-bit unsigned value used for Unicode text. JString is an array of these.
*/

typedef uns16 JChar;



} } // namespace jude::base

#endif // _JUDE_BASE_CHAR_H

