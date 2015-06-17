
// jmutex_win32.h

#include "jwindows.h"

namespace jude {
namespace base {

// this prevents Doxygen duplicating the function descriptions by making it ignore the code
/// @cond



class JMutex {

  CRITICAL_SECTION cs;

public:

  JMutex() {
    InitializeCriticalSection(&cs);
  }

  ~JMutex() {
    DeleteCriticalSection(&cs);
  }

  void lock() {
    EnterCriticalSection(&cs);
  }

  void unlock() {
    LeaveCriticalSection(&cs);
  }

    /*! \brief Non-blocking attempt to lock the mutex. Returns true if the lock was obtained. */
  bool tryLock() { 
    //return (pthread_mutex_trylock(&mutex) == 0); 
    return true;
  }
};



/// @endcond

} } // namespace jude::base

