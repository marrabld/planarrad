
// jclock_win32.h

#include "jwindows.h"
#include "jtypes.h"

namespace jude {
namespace base {

// this prevents Doxygen duplicating the function descriptions by making it ignore the code
/// @cond



class JClock {

  DWORD st;

public:

  JClock() { start(); }

  void start() { st = GetTickCount(); }

  uns32 msecElapsed() const { return GetTickCount() - st; }
};



/// @endcond

} } // namespace jude::base

