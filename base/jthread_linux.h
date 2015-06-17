
// jthread_linux.h

#include <pthread.h>
#include "base/jtypes.h"

namespace jude {
namespace base {

// this prevents Doxygen duplicating the function descriptions by making it ignore the code
/// @cond



class JThread {

  pthread_t thread_id;

  bool is_running;

  //protected:

  //void msleep(unsigned long msecs);

  static void* start_routine(void* vp) { 
    JThread* jt((JThread*) vp);
    jt->is_running = true; 
    jt->run(); 
    jt->is_running = false; 
    pthread_exit(0);
    return 0; 
  }

public:

  JThread() : is_running(false) { }
  virtual ~JThread() { }

    // start the thread - i.e. calls run() in another thread
  void start() {
    pthread_create(&thread_id, 0, start_routine, (void*) this);
  }

    // subclass JThread and provide this function to do processing in the thread
  virtual void run() = 0;

  void waitForFinish() const { pthread_join(thread_id, 0); }
 
  bool finished() const { return !is_running; }
  bool running() const { return is_running; }
};



/// @endcond

} } // namespace jude::base

