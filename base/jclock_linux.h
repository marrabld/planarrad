
// jclock_linux.h

#include <sys/time.h>
#include "base/jtypes.h"

namespace jude {
namespace base {

// this prevents Doxygen duplicating the function descriptions by making it ignore the code
/// @cond


class JClock {

  struct timeval st;
  mutable struct timeval ct;

public:

  JClock() { start(); }

  void start() { gettimeofday(&st,0); }

  uns32 msecElapsed() const {
    gettimeofday(&ct,0); 
    return (ct.tv_sec-st.tv_sec)*1000 + (ct.tv_usec-st.tv_usec)/1000;
  }
};



/// @endcond

} } // namespace jude::base

