
// jmutex_linux.h

#include <pthread.h>

namespace jude {
namespace base {

/*! \brief Permits serialised access to resources in multi-threaded environment. 

Code which may result in errors if multiple threads access it asynchronously can be made thread-safe by the use of a JMutex.
A single JMutex should be provided that a thread must call lock() on before entering the code to be serialised.
When the serialised region of code is reached unlock() is called and other threads may enter.
The function lock() is bloicking, it will not return while another thread has the mutex locked. 
The functon tryLock() is also available, this always returns immediately. 
If the return value is true the mutex has been locked, otherwise it is currently locked by another thread.
*/

class JMutex {

  pthread_mutex_t mutex;

public:

  JMutex() {
    pthread_mutex_init(&mutex, 0);
  }

  ~JMutex() {
    pthread_mutex_destroy(&mutex);
  }

  void lock() {
    pthread_mutex_lock(&mutex);
  }

  void unlock() {
    pthread_mutex_unlock(&mutex);
  }

    /*! \brief Non-blocking attempt to lock the mutex. Returns true if the lock was obtained. */
  bool tryLock() { 
    return (pthread_mutex_trylock(&mutex) == 0); 
  }
};



} } // namespace jude::base

