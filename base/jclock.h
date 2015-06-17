
// jclock.h

#ifndef _JUDE_BASE_JCLOCK_H
#define _JUDE_BASE_JCLOCK_H


#ifndef WIN32
#include "base/jclock_linux.h"
#else
#include "base/jclock_win32.h"
#endif



namespace jude {
namespace base {


/*! \class jude::base::JClock
\brief A class for measuring time in milliseconds.

The JClock counts up constantly in milliseconds from the time it is created. Calling start() resets it to zero time.

Under Linux the JClock is implemented using gettimeofday() whereas under MS Windows GetTickCount() is used. 
The times reported by JClock are therefore as accurate as those functions and subject to the same limitations.
*/


/*! \fn jude::base::JClock::start()
\brief Resets the JClock to time of zero. Time immediately starts counting up.
*/


/*! \fn uns32 jude::base::JClock::msecElapsed() const
\brief Returns the number of milliseconds that have elsaped since the JClock was created or the last call to start().
*/


} } // namespace jude::base



#endif // _JUDE_BASE_JCLOCK_H

